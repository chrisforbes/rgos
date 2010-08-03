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

void kmain( int magic, struct multiboot_info const * info )
{
	vga_clear();
	vga_puts( "RGOS2, booting.\n" );
	
	gdt_init();
	put_status_line( 1, "Descriptor tables configured." );
	vga_puts( "magic=" ); vga_put_hex( (u32) magic ); vga_puts( "\n" );
	vga_puts( "info=" ); vga_put_hex( (u32) info ); vga_puts( "\n" );

	put_status_line( 1, "Setting up PIT..." );	
	timer_init( 50 );
	
	enable_interrupts();
	
	for(;;)
		halt();
}
