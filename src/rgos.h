#pragma once

#define __unused __attribute__((unused))
#define __packed __attribute__((packed))

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u08;

/* from gdt.h */
void gdt_init( void );
