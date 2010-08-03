#include "rgos.h"

void * kmemset( void * s, int c, int n )
{
	u08 * p = s;
	while( n-- ) *p++ = c;
	return s;
}

void outb( u16 port, u08 val ) { asm volatile ("outb %1, %0" : : "dN" (port), "a" (val) ); }
void outw( u16 port, u16 val ) { asm volatile ("outw %1, %0" : : "dN" (port), "a" (val) ); }
u08 inb( u16 port ) { u08 ret; asm volatile ("inb %1, %0" : "=a" (ret) : "dN" (port) ); return ret; }
u16 inw( u16 port ) { u16 ret; asm volatile ("inw %1, %0" : "=a" (ret) : "dN" (port) ); return ret; }
