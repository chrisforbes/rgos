#include "rgos.h"

static void put_reg( char const * name, u32 val )
{
	vga_puts( name ); vga_put( '=' );
	vga_put_hex( val ); vga_put( ' ' );
}

void panic( char const * file, int line, char const * failure, struct regs * r )
{
	vga_puts( "KERNEL PANIC at " );
	vga_puts( file );
	vga_puts( ":" );
	vga_put_dec( line );
	vga_puts( "\n\t  " );
	vga_puts( failure );
	vga_puts( "\n" );
	
	if (r)
	{	/* dump the reg state: */
		put_reg( " ds", r->ds );
		put_reg( "edi", r->edi );
		put_reg( "esi", r->esi );
		put_reg( "ebp", r->ebp );
		vga_put( '\n' );
		put_reg( "esp", r->esp );
		put_reg( "ebx", r->ebx );
		put_reg( "edx", r->edx );
		put_reg( "ecx", r->ecx );
		vga_put( '\n' );
		put_reg( "eax", r->eax );
		put_reg( "int_no", r->int_no );
		put_reg( "err_code", r->err_code );
		put_reg( "eip", r->eip );
		vga_put( '\n' );
		put_reg( "cs", r->cs );
		put_reg( "eflags", r->eflags );
		put_reg( "user_esp", r->user_esp );
		put_reg( "user_ss", r->user_ss );
	}
	
	for(;;)
	{
		disable_interrupts();
		halt();
	}
}
