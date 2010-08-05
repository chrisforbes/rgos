#include "rgos.h"
#include "multiboot.h"


static void put_status_line( u08 ok __unused, char const * msg )
{
	vga_setcolor( 0x0b ); 
	vga_puts( "* " ); 
	vga_setcolor( 0x0f ); 
	vga_puts( msg );
	vga_put( '\n' );
}


void kmain( struct multiboot_info * info __unused )
{
	page_init();	/* we start up with a hacked segment base, so */
	gdt_init();		/* get paging enabled and a real GDT installed first. */
	
	vga_clear();
	put_status_line( 1, "Paging enabled." );
	put_status_line( 1, "Starting Physical Memory Allocator..." );
	phys_alloc_init();
	
	put_status_line( 1, "Starting Kernel Heap Allocator..." );
	kmalloc_init();
	
	/* install other default handlers */
	
	timer_init( 50 );
	
	/* finished initializing, so turn on the interrupts */
	enable_interrupts();
	
	asm volatile( "int $0x3" );
	
	for(;;)
		halt();
}
