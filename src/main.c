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

void kmain( int magic __unused, struct multiboot_header const * header __unused )
{
	vga_clear();
	vga_puts( "RGOS2, booting.\n" );
	
	gdt_init();
	put_status_line( 1, "Descriptor tables configured." );
}
