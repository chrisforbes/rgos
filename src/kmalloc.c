#include "rgos.h"

static u08 * heap_base = (u08 *)0xc8000000;		/* place the kernel heap a bit above the kernel code area. */
static u32 heap_size = 0;
static struct oarray heap_index;

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

#define HEAP_INDEX_SIZE		(32*1024)
#define INITIAL_HEAP_SIZE	(512*1024)
#define B_HOLE	1
#define B_USED	0
#define HEAP_MAGIC	0x55aa55aa

/* todo: contract the heap when (1) there's free space at the end, and (2) it's bigger than MIN_HEAP_SIZE */

/* assumes start..end is unused heap space, or whatever we're replacing doesn't matter. */
static void kmalloc_make_header( u08 * start, u08 * end, u08 flags )
{
	struct block_header * h = (struct block_header *)(start);
	struct block_footer * f = (struct block_footer *)(end - sizeof( *f ));
	
	h->magic = HEAP_MAGIC;
	h->flags = flags;
	h->size = end - start;
	
	f->magic = HEAP_MAGIC;
	f->header = h;
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

static int less_by_size( void * a, void * b )
{
	return ((struct block_header *)a)->size < ((struct block_header *)b)->size;
}

void kmalloc_init( void )
{
	 kmalloc_expand_heap( INITIAL_HEAP_SIZE );
	 
	 // some fiddly crap to bootstrap the index, as an allocation within the
	 // heap we're just creating...
	 
	 u08 * end_of_index = heap_base + sizeof( struct block_header ) 
	 	+ HEAP_INDEX_SIZE + sizeof( struct block_footer );
	 
	 kmalloc_make_header( heap_base, end_of_index, B_USED );
	 kmalloc_make_header( end_of_index, heap_base + heap_size, B_HOLE );
	 	
	 heap_index = oarray_place( heap_base + sizeof( struct block_header ), 
	 	HEAP_INDEX_SIZE, less_by_size );
	 	
	 oarray_insert( &heap_index, end_of_index );
}

void * kmalloc( u32 size __unused )
{
	/* todo! */
	return 0;
}

void kfree( void * p )
{
	struct block_header * h = (void *)((u08 *)p - sizeof(*h));
	struct block_footer * f = (void *)((u08 *)h + h->size - sizeof(*f));
	
	if (h->magic != f->magic)
		PANIC( "Heap corruption: header and footer of block do not match.", 0 );
	if (f->header != h)
		PANIC( "Heap corruption: footer does not point to own header.", 0 );
		
	/* todo */
}


