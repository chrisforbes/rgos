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

extern void gdt_flush(u32 a);	// asm function

static struct gdt_entry gdt[5];
static struct gdt_ptr pgdt;

// todo: idt

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


