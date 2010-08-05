#pragma once

#define __unused __attribute__((unused))
#define __packed __attribute__((packed))
#define __noreturn __attribute__((noreturn))
#define __pagealigned __attribute__((aligned (4096) ))

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u08;

/* from loader.s */
void halt( void );

/* from gdt.c */
void gdt_init( void );

/* from panic.c */
#define PANIC( x ) __PANIC( __FILE__, __LINE__, x )
#define __PANIC( x,y,z ) panic( x, #y, z )
void panic( char const *, char const *, char const * ) __noreturn;

/* from isr.c */
struct regs
{
	u32 ds;
	u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
	u32 int_no, err_code;
	u32 eip, cs, eflags, user_esp, user_ss;
} __packed;

typedef void handler_f( struct regs * );
void isr_register( int int_no, handler_f * f );

void enable_interrupts(void);
void disable_interrupts(void);

#define INT(x)  (x)
#define IRQ(x)	((x) + 32)

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

/* from timer.c */
void timer_init( u32 freq );

/* from paging.c */

//struct pagedir;
//struct page;

void page_init( void );
//void page_flush( struct pagedir * d );
//struct page * page_get( u32 addr, int make, struct pagedir * d );


