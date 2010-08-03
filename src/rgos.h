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
};

void * kmemset( void * s, int c, int n );
