#pragma once

#define __unused __attribute__((unused))
#define __packed __attribute__((packed))
#define __noreturn __attribute__((noreturn))
#define __pagealigned __attribute__((aligned (4096) ))

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u08;

typedef signed char s08;
typedef signed short s16;
typedef signed int s32;

/* from gdt.c */
void gdt_init( void );

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
void halt( void );

#define INT(x)  (x)
#define IRQ(x)	((x) + 32)

/* from panic.c */
#define PANIC( x, r ) panic( __FILE__, __LINE__, x, r )
void panic( char const *, int, char const *, struct regs * ) __noreturn;

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
void vga_put_dec( u32 x );

/* from timer.c */
void timer_init( u32 freq );

/* from paging.c */

//struct pagedir;
//struct page;

void page_init( void );
u32 page_get_phys( void * virt );
void page_commit( void * virt, u32 flags );
//void page_flush( struct pagedir * d );
//struct page * page_get( u32 addr, int make, struct pagedir * d );

/* from phys_alloc.c */
void phys_alloc_init();
u32 phys_alloc_alloc( void );
void phys_alloc_free( u32 frame );
void phys_alloc_stats( void );

/* from kmalloc.c */
void kmalloc_init( void );
void * kmalloc( u32 size );
void kfree( void * p );

/* from ordered_array.c */
struct oarray
{
	void ** data;
	u32 size;
	u32 max_size;
	int (*less)( void * a, void * b );
};

struct oarray oarray_place( void * where, u32 max_size, int (*less)( void * a, void * b ) );
void oarray_insert( struct oarray * o, void * x );
void * oarray_lookup( struct oarray * o, u32 i );
void oarray_remove( struct oarray * o, u32 i );
s32 oarray_getindex( struct oarray * o, void * x );




