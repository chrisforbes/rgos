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
#define MAX_HEAP_SIZE		(8192*1024)					/* allow up to 8M kernel heap */
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

static struct block_header * kmalloc_get_prev( struct block_header * h )
{
	if ((u08 *)h <= heap_base)
		return 0;		// first chunk in the heap
		
	struct block_footer * f = ((struct block_footer *)h) - 1;
	return f->header;
}

/*
// needed for right-unification.
static struct block_header * kmalloc_get_next( struct block_header * h )
{
	struct block_header * hh = (void *)((u32)h + h->size);
	if ((u08 *)hh >= heap_base + heap_size)
		return 0;		// fell off the top of the heap
		
	return hh;
}
*/

static void kmalloc_expand_heap( u32 new_size )
{
	u08 * start = heap_base + heap_size;
	u08 * end = heap_base + new_size;
	u08 * p;
	
	for( p = start; p != end; p += 0x1000 )
		page_commit( p, 0x3 );
		
	kmalloc_make_header( start, end, B_HOLE );	/* mark the whole thing free */
	heap_size = new_size;
	if (heap_index.data)
	{
		struct block_header * self = (struct block_header *) start;
		struct block_header * prev = kmalloc_get_prev( self );
		
		if (prev && prev->flags == B_HOLE)
		{
			/* merge */
			s32 previndex = oarray_getindex( &heap_index, prev );
			oarray_remove( &heap_index, previndex );	/* cheat: we know it's valid. */
			
			prev->size = prev->size + self->size;
			struct block_footer * f = (void *)(end - sizeof( *f ));
			f->header = prev;
			
			oarray_insert( &heap_index, prev );			/* reinsert the merged block */
			
			vga_puts( "Merged final heap block -> " ); vga_put_dec( prev->size >> 10 ); vga_puts( "KB\n" );
		}
		else
			oarray_insert( &heap_index, start );
	}
	
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

static s32 find_smallest_hole( u32 size )
{
	u32 it = 0;
	while( it < heap_index.size )
	{
		struct block_header * h = oarray_lookup( &heap_index, it );
		if (h->size >= size)
			break;		/* fits */
		
		++it;
	}
	
	return it < heap_index.size ? (s32)it : -1;
}

void * kmalloc( u32 size )
{
	u32 new_size = size + sizeof(struct block_header) + sizeof(struct block_footer);
	s32 it = find_smallest_hole( new_size );
	
	if (it == -1)	/* alloc failure: try expanding the heap. */
	{
		kmalloc_expand_heap( 2 * heap_size );
		return kmalloc( size );
	}
	
	struct block_header * h = oarray_lookup( &heap_index, it );
	u32 orig_hole_pos = (u32) h;
	u32 orig_hole_size = h->size;
	
	if (orig_hole_size - new_size <= sizeof( struct block_header ) + sizeof( struct block_footer ))
	{
		/* the leftover space is too small -- just expand this allocation to consume it. */
		size += orig_hole_size - new_size;
		new_size = orig_hole_size;
	}
	
	oarray_remove( &heap_index, it );
	
	h->magic = HEAP_MAGIC;
	h->flags = B_USED;
	h->size = new_size;
	
	struct block_footer * f = (void *)(orig_hole_pos + sizeof( *h ) + new_size);
	f->magic = HEAP_MAGIC;
	f->header = h;
	
	if (orig_hole_size - new_size > 0)
	{
		struct block_header * hh = (struct block_header *)( f+1 );
		hh->magic = HEAP_MAGIC;
		hh->flags = B_HOLE;
		hh->size = orig_hole_size - new_size;
		struct block_footer * ff = (struct block_footer *)( orig_hole_pos + orig_hole_size - sizeof( *ff ));
		ff->magic = HEAP_MAGIC;
		ff->header = hh;
		
		oarray_insert( &heap_index, hh );
	}
	
	return (void *)(h+1);	/* just after the header */
}

void kfree( void * p )
{
	struct block_header * h = (void *)((u08 *)p - sizeof(*h));
	struct block_footer * f = (void *)((u08 *)h + h->size - sizeof(*f));
	
	if (h->magic != HEAP_MAGIC)
		PANIC( "Heap corruption: header magic is broken", 0 );
	if (f->magic != HEAP_MAGIC)
		PANIC( "Heap corruption: footer magic is broken.", 0 );
	if (f->header != h)
		PANIC( "Heap corruption: footer does not point to own header.", 0 );
		
	/* todo */
}


