#include "rgos.h"

static u32 kernelpagedir[1024] __pagealigned;
static u32 lowpagetable[1024] __pagealigned;

static void pagefault_handler( struct regs * regs __unused )
{
	u32 faulting_address;
	asm volatile ( "mov %%cr2, %0" : "=r"( faulting_address ) );
	vga_puts( "Page fault! addr=" );
	vga_put_hex( faulting_address );
	vga_puts( "\n" );
	
	for(;;)
	{
		disable_interrupts();
		halt();
	}
}

void page_init( void )
{
	void * kernelpagedir_ptr = (char *) kernelpagedir + 0x40000000;
	void * lowpagetable_ptr = (char *) lowpagetable + 0x40000000;
	int k;
	
	for( k = 0; k < 1024; k++ )
	{
		lowpagetable[k] = (k << 12) | 0x3;
		kernelpagedir[k] = 0;
	}
	
	kernelpagedir[0] = (u32)lowpagetable_ptr | 0x3;
	kernelpagedir[768] = (u32)lowpagetable_ptr | 0x3;
	
	// map the kernelpagedir as the last pagetable,
	// to facilitate easy determination of physical addresses.
	kernelpagedir[1023] = (u32)kernelpagedir_ptr | 0x3;			
	
	// set cr3 and enable paging.
	asm volatile (	
		"mov %0, %%eax\n"
		"mov %%eax, %%cr3\n"
		"mov %%cr0, %%eax\n"
		"orl $0x80000000, %%eax\n"
		"mov %%eax, %%cr0\n" :: "m" (kernelpagedir_ptr));
		
	isr_register( INT(14), pagefault_handler );
}

u32 page_get_phys( void * virt )
{
	u32 pdindex = (u32)virt >> 22;
	u32 ptindex = ((u32)virt >> 12) & 0x3ff;
	
	u32 * pd = (u32 *)0xfffff000;		/* due to last-pagetable reflection, this is fixed. */
	u32 * pt = (u32 *)0xffc00000 + 0x400 * pdindex;
	
	if( ~pd[pdindex] & 1 ) return 0;	/* pagetable not present */
	if( ~pt[ptindex] & 1 ) return 0;	/* page not present */
	
	return (pt[ptindex] & ~0x0fff) + ((u32)virt & 0x0fff);
}

static void flush_tlb(void)
{
	void * kernelpagedir_ptr = (char *) kernelpagedir + 0x40000000;
	asm volatile (
		"mov %0, %%eax\n"
		"mov %%eax, %%cr3" :: "m" (kernelpagedir_ptr) );		/* flush the TLB */
}

/* really only for kmalloc to use. */
void page_commit( void * virt, u32 flags )
{
	u32 pdindex = (u32)virt >> 22;
	u32 ptindex = ((u32)virt >> 12) & 0x3ff;
	
	u32 * pd = (u32 *)0xfffff000;
	u32 * pt = (u32 *)0xffc00000 + 0x400 * pdindex;

	if( ~pd[pdindex] & 1 )
	{
		/* the pagetable needs created. */
		u32 page_for_pd = phys_alloc_alloc();
		pd[pdindex] = (page_for_pd << 12) | 0x3;	/* present! */
		kmemset( pt, 0, 0x1000 );
	}
	
	if ( ~pt[ptindex] & 1 )		
	{
		/* the page needs created (this is the GOOD case!) */
		u32 phys_page = phys_alloc_alloc();
		pt[ptindex] = (phys_page << 12) | (flags & 0xfff) | 0x1;	/* force present! */	
		flush_tlb();
		return;
	}
	else
		PANIC( "Mapping already exists!", 0 );	/* probably shouldn't panic, but... */
}


