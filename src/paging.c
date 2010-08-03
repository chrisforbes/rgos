#include "rgos.h"

struct page
{
	u32 present:1,
		rw:1,
		user:1,
		accessed:1,
		dirty:1,
		unused:7,
		frame:20;
};

struct pagetable
{
	struct page pages[1024];
};

struct pagedir
{
	struct pagetable * tables[1024];
	u32 phys_tables[1024];
	u32 phys;		/* interesting very soon */
};

extern u32 placement_addr;	/* from kheap.c */

u32 * frames;		/* bitset */
u32 nframes;

#define INDEX_FROM_BIT(a) ((a) >> 5)
#define OFFSET_FROM_BIT(a) ((a) & 31)

static void frame_set( u32 addr )
{
	u32 frame = addr >> 12;
	frames[ INDEX_FROM_BIT(frame) ] |= (1 << OFFSET_FROM_BIT(frame));
}

/*
static void frame_clear( u32 addr )
{
	u32 frame = addr >> 12;
	frames[ INDEX_FROM_BIT(frame) ] &= ~(1 << OFFSET_FROM_BIT(frame));
}
*/

static u32 frame_firstfree( void )
{
	u32 i,j;
	for( i = 0; i < INDEX_FROM_BIT(nframes); i++ )
		if (!~frames[i])		/* if all frames in this chunk are alloc'd,.. */
			for( j = 0; j < 32; j++ )
				if (! (frames[i] & (1<<j) ))
					return i * 32 + j;
					
	PANIC( "No frames!" );
}

static void frame_alloc( struct page * page, int is_kernel, int is_writable )
{
	if (page->frame) return;	/* already allocated, out! */
	
	u32 idx = frame_firstfree();
	frame_set( idx << 12 );		/* mark it allocated */
	page->present = 1;
	page->rw = !!is_writable;
	page->user = !is_kernel;
	page->frame = idx;
}

/*
static void frame_free( struct page * page )
{
	if (!page->frame) return;
	frame_clear( page->frame );
	page->frame = 0;
}
*/

struct pagedir * kernel_directory;
struct pagedir * current_directory;

void page_fault_handler( struct regs * r __unused )
{
	PANIC( "Page Fault" );
}

struct page * page_get( u32 addr, int make, struct pagedir * d )
{
	addr >>= 12;
	u32 idx = addr >> 12;
	
	if (d->tables[ idx ])
		return &d->tables[idx]->pages[ addr & 0xfff ];
	else if (make)
	{
		u32 phys;
		d->tables[idx] = kmalloc_aligned_phys( sizeof( struct pagetable ), &phys );
		kmemset( d->tables[idx], 0, 0x1000 );
		d->phys_tables[idx] = phys | 0x7;
		return &d->tables[idx]->pages[ addr & 0xfff ];
	}
	else
		return 0;
}

void page_init( void )
{
	u32 end_of_mem = 0x1000000;	/* 64M for now -- this should really use the memory map from grub! */
	nframes = end_of_mem >> 12;
	frames = kmalloc( INDEX_FROM_BIT(nframes) );
	kmemset( frames, 0, INDEX_FROM_BIT( nframes ) );
	
	// construct a page directory
	kernel_directory = kmalloc_aligned( sizeof( *kernel_directory ) );
	kmemset( kernel_directory, 0, sizeof( *kernel_directory ) );
	
	// identity-map!
	u32 i;
	while( i < placement_addr )
	{
		frame_alloc( page_get( i, 1, kernel_directory ), 0, 0 );
		i += 0x1000;
	}
	
	isr_register( INT(14), page_fault_handler );
	page_flush( kernel_directory );
	
	u32 cr0;
	asm volatile( "mov %%cr0, %0" : "=r"(cr0));
	cr0 |= 0x80000000;	// enable paging
	asm volatile( "mov %0, %%cr0" :: "r"(cr0));
}

void page_flush( struct pagedir * d )
{
	current_directory = d;
	asm volatile( "mov %0, %%cr3" :: "r"(&d->phys_tables) );
}


