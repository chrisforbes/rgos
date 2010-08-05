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

static void kmalloc_expand_heap( u32 new_size )
{
	u08 * start = heap_base + heap_size;
	u08 * end = heap_base + new_size;
	u08 * p;
	
	for( p = start; p != end; p += 0x1000 )
		page_commit( p, 0x3 );
		
	struct block_header * new_block_header = (struct block_header *)(start);
	struct block_footer * new_block_footer = (struct block_footer *)(end - sizeof( *new_block_footer ));
	
	new_block_header->magic = next_magic;
	new_block_header->flags = B_HOLE;
	new_block_header->size = end - start;
	
	new_block_footer->magic = next_magic++;
	new_block_footer->header = new_block_header;
		
	heap_size = new_size;
	
	vga_puts( "Heap expanded to " ); vga_put_dec( heap_size >> 10 ); vga_puts( "KB\n" );
	phys_alloc_stats();
}

void kmalloc_init( void )
{
	 kmalloc_expand_heap( INITIAL_HEAP_SIZE );
}

void * kmalloc( u32 size );
void kfree( void * p );
