#include "rgos.h"

/* for now, assume we have 64M of memory : 16K physical pages */
/* todo: use multiboot info to build a real memory map that works. */
static u32 phys_bitmap[ 512 ];
static u32 free_pages = 16384;

static int phys_isset( u32 page ) { return phys_bitmap[ page >> 5 ] & (1 << (page & 0x1f)); }
static void phys_set( u32 page ) { phys_bitmap[ page >> 5 ] |= (1 << (page & 0x1f)); --free_pages; }
static void phys_clear( u32 page ) { phys_bitmap[ page >> 5 ] &= ~(1 << (page & 0x1f)); ++free_pages; }

extern u32 end;	/* provided by linker script */
static u32 next_frame = 0;	/* the lowest possible frame that is free. */

void phys_alloc_init( void )
{
	/* initially, everything from 0 to `end` - 0xc0000000 is mapped! */ 
	u32 i;
	u32 end_of_kernel_phys = (u32)&end - 0xc0000000;
	
	u32 j = 0;
	for( i = 0; i < end_of_kernel_phys; i+=0x1000, j++ )
		phys_set( i >> 12 );
		
	vga_puts( "Initial kernel region: " );
	vga_put_dec( end_of_kernel_phys >> 10 );
	vga_puts( "KB\n" );
	vga_puts( "Physical pages allocated: " );
	vga_put_dec( j );
	vga_puts( "\nAvailable Physical Memory: " );
	vga_put_dec( free_pages * 4 );
	vga_puts( "KB\n" );
}

u32 phys_alloc_alloc( void )	/* allocates a new page frame, and returns the index. */
{
	u32 i, j;
	for( i = next_frame; i < sizeof(phys_bitmap) / sizeof( *phys_bitmap ); i++ )
		if (~phys_bitmap[i])
			for( j = 0; j < 32; j++ )
				if (~phys_bitmap[i] & (1<<j))
				{
					u32 frame = i * 32 + j;
					vga_puts( "Allocating frame: " ); vga_put_hex( frame );
					phys_set( frame );
					next_frame = frame + 1;
					return frame;
				}
	
	PANIC( "Physical memory exhausted.", 0 );
}

void phys_alloc_free( u32 frame )
{
	if (!phys_isset( frame ))
		PANIC( "Double-free of physical memory", 0 );
		
	if (frame < next_frame)
		next_frame = frame;		/* so we can quickly satisfy another request */
	
	phys_clear( frame );
}
