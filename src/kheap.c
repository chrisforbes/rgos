#include "rgos.h"

u32 placement_addr = 0;

static void * kmalloc_internal( u32 size, int align, u32 * phys )
{
	if (align && (placement_addr & ~0x0fff))
	{
		placement_addr &= ~0x0fff;
		placement_addr += 0x1000;
	}
	
	if (phys)
		*phys = placement_addr;
	
	u32 ret = placement_addr;
	placement_addr += size;
	return (void *)ret;
}

void * kmalloc( u32 size ) 
	{ return kmalloc_internal( size, 0, 0 ); }
void * kmalloc_aligned( u32 size ) 
	{ return kmalloc_internal( size, 1, 0 ); }
void * kmalloc_phys( u32 size, u32 * phys /*out*/ ) 
	{ return kmalloc_internal( size, 0, phys ); }
void * kmalloc_aligned_phys( u32 size, u32 * phys /*out*/ ) 
	{ return kmalloc_internal( size, 1, phys ); }
