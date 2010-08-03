#include "rgos.h"

struct gdt_entry
{
	u16 limit_low, base_low;
	u08 base_middle, access, granularity, base_high;
} __packed;

struct gdt_ptr
{
	u16 limit;
	u32 base;
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
	u32 base;
} __packed;

extern void gdt_flush(u32 a);	// asm function
extern void idt_flush(u32 a);	// asm function

static struct gdt_entry gdt[5];
static struct gdt_ptr pgdt;

static struct idt_entry idt[256];
static struct idt_ptr pidt;

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

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

static void idt_set_gate( int index, u32 base, u16 sel, u08 flags )
{
	struct idt_entry * i = idt+index;
	i->base_lo = base & 0xffff;
	i->base_hi = (base >> 16) & 0xffff;
	i->sel = sel;
	i->sbz = 0;
	i->flags = flags;	/* | 0x60 */ /* for access from usermode */
}

void gdt_init( void )
{
	pgdt.limit = sizeof(gdt) - 1;
	pgdt.base = (u32)(void *) gdt;

	gdt_set_gate( 0, 0, 0, 0, 0 );	// null segment
	gdt_set_gate( 1, 0, 0xffffffffu, 0x9a, 0xcf );	// code
	gdt_set_gate( 2, 0, 0xffffffffu, 0x92, 0xcf );	// data
	gdt_set_gate( 3, 0, 0xffffffffu, 0xfa, 0xcf );	// usermode code
	gdt_set_gate( 4, 0, 0xffffffffu, 0xf2, 0xcf );	// usermode data

	gdt_flush( (u32)(void *) &pgdt );
}


