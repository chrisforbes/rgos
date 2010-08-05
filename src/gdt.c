#include "rgos.h"

struct gdt_entry
{
	u16 limit_low, base_low;
	u08 base_middle, access, granularity, base_high;
} __packed;

struct gdt_ptr
{
	u16 limit;
	struct gdt_entry const * base;
} __packed;

struct idt_entry
{
	u16 base_low, sel;
	u08 sbz, flags;
	u16 base_high;
} __packed;

struct idt_ptr
{
	u16 limit;
	struct idt_entry const * base;
} __packed;

extern void gdt_flush( struct gdt_ptr const * p );	// asm function
extern void idt_flush( struct idt_ptr const * p );	// asm function

static struct gdt_entry gdt[5];
static struct gdt_ptr pgdt;

static struct idt_entry idt[256];
static struct idt_ptr pidt;

extern void isr0( void ); extern void isr1(); extern void isr2(); extern void isr3();
extern void isr4(); extern void isr5(); extern void isr6(); extern void isr7();
extern void isr8(); extern void isr9(); extern void isr10(); extern void isr11();
extern void isr12(); extern void isr13(); extern void isr14(); extern void isr15();
extern void isr16(); extern void isr17(); extern void isr18(); extern void isr19();
extern void isr20(); extern void isr21(); extern void isr22(); extern void isr23();
extern void isr24(); extern void isr25(); extern void isr26(); extern void isr27();
extern void isr28(); extern void isr29(); extern void isr30(); extern void isr31();

extern void irq0(); extern void irq1(); extern void irq2(); extern void irq3();
extern void irq4(); extern void irq5(); extern void irq6(); extern void irq7();
extern void irq8(); extern void irq9(); extern void irq10(); extern void irq11();
extern void irq12(); extern void irq13(); extern void irq14(); extern void irq15();

static void gdt_set_gate( int index, u32 base, u32 limit, u08 access, u08 granularity )
{
	struct gdt_entry * g = gdt+index;
	g->base_low = (base & 0xffff);
	g->base_middle = (base >> 16) & 0xff;
	g->base_high = (base >> 24) & 0xff;
	g->limit_low = (limit & 0xffff);
	g->granularity = ((limit >> 16) & 0x0f) | (granularity & 0xf0);
	g->access = access;
}


static void idt_set_gate( int index, void (*pf), u16 sel, u08 flags )
{
	u32 base = (u32) pf;
	struct idt_entry * i = idt+index;
	i->base_low = base & 0xffff;
	i->base_high = (base >> 16) & 0xffff;
	i->sel = sel;
	i->sbz = 0;
	i->flags = flags;	// | 0x60 [usermode]
}


void gdt_init( void )
{
	pgdt.limit = sizeof(gdt) - 1;
	pgdt.base = gdt;

	gdt_set_gate( 0, 0, 0, 0, 0 );	// null segment
	gdt_set_gate( 1, 0, 0xffffffffu, 0x9a, 0xcf );	// code
	gdt_set_gate( 2, 0, 0xffffffffu, 0x92, 0xcf );	// data
	gdt_set_gate( 3, 0, 0xffffffffu, 0xfa, 0xcf );	// usermode code
	gdt_set_gate( 4, 0, 0xffffffffu, 0xf2, 0xcf );	// usermode data

	gdt_flush( &pgdt );
	
	kmemset( idt, 0, sizeof(idt) );
	
	pidt.limit = sizeof(idt) - 1;
	pidt.base = idt;
	
	// remap the pic IRQ0-15 => INT32-47
	outb( 0x20, 0x11 );
	outb( 0xa0, 0x11 );
	outb( 0x21, 0x20 );
	outb( 0xa1, 0x28 );
	outb( 0x21, 0x04 );
	outb( 0xa1, 0x02 );
	outb( 0x21, 0x01 );
	outb( 0xa1, 0x01 );
	outb( 0x21, 0x0 );
	outb( 0xa1, 0x0 );
	
	typedef void isr();
	isr* isrs[] = { isr0, isr1, isr2, isr3, isr4, isr5, isr6, isr7, 
					isr8, isr9, isr10, isr11, isr12, isr13, isr14, isr15, 
					isr16, isr17, isr18, isr19, isr20, isr21, isr22, isr23, 
					isr24, isr25, isr26, isr27, isr28, isr29, isr30, isr31,
					
					irq0, irq1, irq2, irq3, irq4, irq5, irq6, irq7, 
					irq8, irq9, irq10, irq11, irq12, irq13, irq14, irq15 };
	
	int i;
	for( i = 0; i < (int)(sizeof(isrs) / sizeof(*isrs)); i++ )
		idt_set_gate( i, isrs[i], 0x08, 0x8e );
	
	idt_flush( &pidt );
	
}


