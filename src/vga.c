#include "rgos.h"

/* todo: completely rewrite this on the device framework once that exists. */

#define WIDTH 80
#define HEIGHT 25
#define TABSIZE 4

static u16 * fb = (u16 *) 0xc00b8000;
static u08 attrib = 0x0f;
static u08 x = 0, y = 0;

/* scrolls the text framebuffer up one line */
static void vga_scroll( void )
{
	u16 blank = ' ' | (attrib << 8);
	if (y >= HEIGHT)
	{
		int i;
		for( i = 0; i < WIDTH * (HEIGHT - 1); i++ )
			fb[ i ] = fb[ i + WIDTH ];
		for( i = WIDTH * (HEIGHT - 1); i < WIDTH * HEIGHT; i++ )
			fb[ i ] = blank;
			
		y = 24;
	}
}

/* moves the vga hardware cursor */
static void vga_set_cursor( void )
{
	u16 location = y * WIDTH + x;
	outb( 0x3d4, 14 );
	outb( 0x3d5, location >> 8 );
	outb( 0x3d4, 15 );
	outb( 0x3d5, location );
}

void vga_put( char c )
{
	u16 val = c | (attrib << 8);

	if ((c == '\b') && x) --x;
	else if (c == '\t')	x = (x + TABSIZE) & ~(TABSIZE-1);
	else if (c == '\r') x = 0;
	else if (c == '\n')	{ x = 0; ++y; }
	else if (c >= ' ') fb[ y * WIDTH + x++ ] = val;
	
	if (x >= WIDTH) { x = 0; ++y; }
	vga_scroll();
	vga_set_cursor();
}

void vga_clear( void )
{
	u16 blank = ' ' | (attrib << 8);
	int i;
	for( i = 0; i < WIDTH * HEIGHT; i++ )
		fb[ i ] = blank;
		
	x = y = 0;
	vga_set_cursor();
}

void vga_setcolor( u08 c ) { attrib = c; }
void vga_puts( char const * s ) { while( *s ) vga_put( *s++ ); }

void vga_put_dec( u32 x )
{
	char buf[10], *p = buf;
	if (!x)
	{
		vga_puts( "0" );
		return;
	}
	
	while( x )
	{
		*p++ = (x % 10) + '0';
		x /= 10;
	}
	
	while( p > buf )
		vga_put( *--p );
}

void vga_put_hex( u32 x )
{
	static char hexdigits[] = "0123456789abcdef";
	char buf[9], *p = buf;
	
	vga_puts( "0x" );
	
	if (!x)
	{
		vga_puts( "0" );
		return;
	}
	
	while( x )
	{
		*p++ = hexdigits[ x & 0xf ];
		*p++ = hexdigits[ (x >> 4) & 0xf ];
		x >>= 8;
	}
	
	while( p > buf )
		vga_put( *--p );
}




