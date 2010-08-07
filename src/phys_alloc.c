#include "rgos.h"
#include "multiboot.h"

/* for now, assume we have 64M of memory : 16K physical pages */
/* todo: use multiboot info to build a real memory map that works. */
static u32 phys_bitmap[ 512 ];
static u32 free_pages = 16384;

static int phys_isset( u32 page ) { return phys_bitmap[ page >> 5 ] & (1 << (page & 0x1f)); }
static void phys_set( u32 page ) { phys_bitmap[ page >> 5 ] |= (1 << (page & 0x1f)); --free_pages; }
static void phys_clear( u32 page ) { phys_bitmap[ page >> 5 ] &= ~(1 << (page & 0x1f)); ++free_pages; }

extern u32 end;	/* provided by linker script */
static u32 next_frame = 0;	/* the lowest possible frame that is free. */

void phys_alloc_stats( void )
{
	vga_puts( "Available Physical Memory: " );
	vga_put_dec( free_pages * 4 );
	vga_puts( "KB\n" );
}

void phys_alloc_from_bootinfo( struct multiboot_info * mbh )
{
	vga_puts( "GRUB boot info: " );
	vga_put_hex( (u32) mbh );
	vga_puts( " flags: " );
	vga_put_hex( mbh->flags );
	vga_puts( "\n" );
	
	if (!(mbh->flags & (1<<6)))
		PANIC( "GRUB didn't give us a memory map", 0 );
	
	struct multiboot_mmap * mmap = (void*)mbh->mmap_addr;
	vga_puts( "Reading memory map: " );
	vga_put_hex( (u32) mmap );
	vga_puts( " Length: " );
	vga_put_hex( (u32) mbh->mmap_length );
	vga_puts( "\n" );
}

void phys_alloc_init( struct multiboot_info * mbh )
{
	/* first: setup the memory map based on bootinfo */
	phys_alloc_from_bootinfo( mbh );
	
	/* now reserve the kernel pages */
	/* kernel virtual addresses: 0xc0100000 - &end [linker sym] */
	/* loaded to physical 0x100000 */
	u32 i;
	u32 end_of_kernel_phys = (u32)&end - 0xc0100000;
	
	u32 j = 0;
	for( i = 0x100000; i < end_of_kernel_phys; i+=0x1000, j++ )
		phys_set( i >> 12 );
		
	vga_puts( "Kernel image size: " );
	vga_put_dec( end_of_kernel_phys >> 10 );
	vga_puts( "KB\n" );
	vga_puts( "Physical pages allocated: " );
	vga_put_dec( j ); vga_puts( "\n" );

	phys_alloc_stats();
}

u32 phys_alloc_alloc( void )	/* allocates a new page frame, and returns the index. */
{
	u32 i, j;
	for( i = next_frame / 32; i < sizeof(phys_bitmap) / sizeof( *phys_bitmap ); i++ )
		if (~phys_bitmap[i])
			for( j = 0; j < 32; j++ )
				if (~phys_bitmap[i] & (1<<j))
				{
					u32 frame = i * 32 + j;
			//		vga_puts( "Allocating frame: " ); vga_put_hex( frame ); vga_puts( "\n" );
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
