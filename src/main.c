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


void kmain( struct multiboot_info * info )
{
	page_init();	/* we start up with a hacked segment base, so */
	gdt_init();		/* get paging enabled and a real GDT installed first. */
	
	vga_clear();
	
	put_status_line( 1, "Paging enabled." );
	put_status_line( 1, "Starting Physical Memory Allocator..." );
	phys_alloc_init( info );
	
	put_status_line( 1, "Starting Kernel Heap Allocator..." );
	kmalloc_init();	
	page_init_finish();
	
	/* install other default handlers */
	
	timer_init( 50 );
	
	/* test the heap allocator */
	int * foo = kmalloc( 240 );
	vga_puts( "Allocation test: " );
	vga_put_hex( (u32) foo );
	vga_puts( "\n" );
	
	*foo = 42;	/* shouldn't die */
	
	put_status_line( 1, "Scanning PCI buses..." );
	pci_enum_devices();
	
	/* finished initializing, so turn on the interrupts */
	enable_interrupts();
	
//	asm volatile( "int $0x3" );
	
	for(;;)
		halt();
}
