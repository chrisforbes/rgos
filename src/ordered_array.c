#include "rgos.h"

struct oarray oarray_place( void * where, u32 max_size, int (*less)( void * a, void * b ) )
{
	struct oarray o = { 
		.data = where, 
		.max_size = max_size,
		.size = 0,
		.less = less
	};
	
	return o;
}

/* really slow oarray insert. we should really be using bins! */
void oarray_insert( struct oarray * o, void * x )
{
	u32 it = 0;
	while( it < o->size && o->less( o->data[it], x ) )
		++it;
	if (it == o->size)
		o->data[o->size++] = x;
	else
	{
		void * temp = o->data[it];
		o->data[it] = x;
		while( it++ < o->size )
		{
			void * temp2 = o->data[it];
			o->data[it] = temp;
			temp = temp2;
		}
		
		o->size++;
	}
}

void * oarray_lookup( struct oarray * o, u32 i )
{
	return o->data[i];
}

s32 oarray_getindex( struct oarray * o, void * x )
{
	u32 it = 0;
	while( it < o->size )
		if (o->data[it] == x)
			return it;
		else
			++it;
	
	return -1;
}

void oarray_remove( struct oarray * o, u32 i )
{
	while( i < o->size )
	{
		o->data[i] = o->data[i+1];
		i++;
	}
	
	o->size--;
}

