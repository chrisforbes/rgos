#include "rgos.h"

#define PCI_CONFIG_ADDRESS	0xcf8
#define PCI_CONFIG_DATA		0xcfc

static u32 pci_read_u32( u32 bus, u32 slot, u32 func, u32 offset )
{
	u32 addr = 0x80000000 | (bus << 16) | (slot << 11) | (func << 8) | (offset & 0xfc);
	outl( PCI_CONFIG_ADDRESS, addr );
	return inl( PCI_CONFIG_DATA );
}

static u16 pci_read_u16( u32 bus, u32 slot, u32 func, u32 offset )
{
	return (u16) (pci_read_u32( bus, slot, func, offset ) >> ((offset & 2) * 8));
}

struct pci_slotinfo { u32 bus, slot, func; };
struct pci_devinfo 
{
	u16 vendor_id, device_id, command, status;
	u08 revision_id, prog_if, subclass, class;
	u08 cache_line_size, latency_timer, header_type, bist;
};

struct pci_devdesc
{
	
};

static void pci_dump_devinfo( struct pci_slotinfo const * psi, struct pci_devinfo const * pdi )
{
	vga_put_dec( psi->bus ); vga_puts(":");
	vga_put_dec( psi->slot ); vga_puts(":");
	vga_put_dec( psi->func ); 
	vga_puts(": vid="); vga_put_hex( pdi->vendor_id );
	vga_puts(" did="); vga_put_hex( pdi->device_id );
	vga_puts(" class="); vga_put_hex( pdi->class );
	vga_puts(" subclass="); vga_put_hex( pdi->subclass );
	vga_puts(" prog_if="); vga_put_hex( pdi->prog_if );
	vga_puts("\n");
}

void pci_enum_devices( void )
{
	u32 bus, slot, func;
	for( bus = 0; bus < 256; bus++ )
		for( slot = 0; slot < 32; slot++ )
			for( func = 0; func < 8; func++ )
			{
				if (pci_read_u16( bus, slot, func, 0 ) == 0xffff)
					continue;
					
				struct pci_slotinfo psi = { .bus = bus, .slot = slot, .func = func };
				struct pci_devinfo pdi;
				u32 * p = (u32 *) &pdi;
				u32 i;
				for( i = 0; i < sizeof(pdi); i += sizeof(u32) )
					p[i >> 2] = pci_read_u32( bus, slot, func, i );
					
				pci_dump_devinfo( &psi, &pdi );
			}
}
