#include "multiboot.h"
#include "rgos.h"

static void put_status_line( u08 ok __unused, char const * msg )
{
	vga_setcolor( 0x0b ); 
	vga_puts( "* " ); 
	vga_setcolor( 0x0f ); 
	vga_puts( msg );
	vga_put( '\n' );
}

void kmain( int magic, struct multiboot_header const * header )
{
	vga_clear();
	vga_puts( "RGOS2, booting.\n" );
	
	gdt_init();
	put_status_line( 1, "Descriptor tables configured." );
	vga_puts( "magic=" ); vga_put_hex( (u32) magic ); vga_puts( "\n" );
	vga_puts( "header=" ); vga_put_hex( (u32) header ); vga_puts( "\n" );

	put_status_line( 1, "Testing interrupts now..." );	
	/* produce some spurious interrupts */
	asm volatile( "int $0x3" );
	asm volatile( "int $0x4" );
	
	put_status_line( 1, "Interrupt test completed." );
}
