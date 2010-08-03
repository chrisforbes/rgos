#pragma once

#define __unused __attribute__((unused))
#define __packed __attribute__((packed))

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u08;

/* from gdt.c */
void gdt_init( void );

/* from isr.c */
struct regs
{
	u32 ds;
	u32 edi, esi, esp, ebx, edx, ecx, eax;
	u32 int_no, err_code;
	u32 eip, cs, eflags, user_esp, user_ss;
} __packed;

/* from kstdlib.c */
void * kmemset( void * s, int c, int n );

void outb( u16 port, u08 val );
u08 inb( u16 port );
void outw( u16 port, u16 val );
u16 inw( u16 port );

/* from vga.c */
void vga_put( char c );
void vga_clear( void );
void vga_puts( char const * s );
void vga_setcolor( u08 c );

void vga_put_hex( u32 x );
