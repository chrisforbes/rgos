#include "rgos.h"

static u08 * heap_base = (u08 *)0xc8000000;		/* place the kernel heap a bit above the kernel code area. */
static u32 heap_size = 0;
static u32 next_magic;

struct block_header
{
	u32 magic;
	u08 flags;
	u32 size;
};

struct block_footer
{
	u32 magic;
	struct block_header * header;
};

#define INITIAL_HEAP_SIZE	(512*1024)
#define B_HOLE	1

/* todo: contract the heap when (1) there's free space at the end, and (2) it's bigger than MIN_HEAP_SIZE */

/* assumes start..end is unused heap space, or whatever we're replacing doesn't matter. */
static void kmalloc_make_header( u08 * start, u08 * end, u08 flags )
{
	struct block_header * new_block_header = (struct block_header *)(start);
	struct block_footer * new_block_footer = (struct block_footer *)(end - sizeof( *new_block_footer ));
	
	new_block_header->magic = next_magic;
	new_block_header->flags = flags;
	new_block_header->size = end - start;
	
	new_block_footer->magic = next_magic++;
	new_block_footer->header = new_block_header;
}

static void kmalloc_expand_heap( u32 new_size )
{
	u08 * start = heap_base + heap_size;
	u08 * end = heap_base + new_size;
	u08 * p;
	
	for( p = start; p != end; p += 0x1000 )
		page_commit( p, 0x3 );
		
	kmalloc_make_header( start, end, B_HOLE );	/* mark the whole thing free */
	heap_size = new_size;
	
	vga_puts( "Heap expanded to " ); vga_put_dec( heap_size >> 10 ); vga_puts( "KB\n" );
	phys_alloc_stats();
}

void kmalloc_init( void )
{
	 kmalloc_expand_heap( INITIAL_HEAP_SIZE );
}

void * kmalloc( u32 size );

void kfree( void * p )
{
	struct block_header * h = (void *)((u08 *)p - sizeof(*h));
	struct block_footer * f = (void *)((u08 *)h + h->size - sizeof(*f));
	
	if (h->magic != f->magic)
		PANIC( "Heap corruption: header and footer of block do not match.", 0 );
}
