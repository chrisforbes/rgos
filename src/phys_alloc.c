#include "rgos.h"

/* for now, assume we have 64M of memory : 16K physical pages */
/* todo: use multiboot info to build a real memory map that works. */
static u32 phys_bitmap[ 512 ];

//static int phys_isset( u32 page ) { return phys_bitmap[ page >> 5 ] & (1 << (page & 0x1f)); }
static void phys_set( u32 page ) { phys_bitmap[ page >> 5 ] |= (1 << (page & 0x1f)); }
//static void phys_clr( u32 page ) { phys_bitmap[ page >> 5 ] &= ~(1 << (page & 0x1f)); }

extern u32 end;	/* provided by linker script */

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
	vga_put( '\n' );
}
