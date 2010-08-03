#include "rgos.h"

void * kmemset( void * s, int c, int n )
{
	u08 * p = s;
	while( n-- ) *p++ = c;
	return s;
}
