.global loader

.set ALIGN,	1<<0
.set MEMINFO,	1<<1
.set FLAGS,	ALIGN | MEMINFO
.set MAGIC,	0x1badb002
.set CHECKSUM, 	-(MAGIC + FLAGS)

.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.set STACKSIZE, 0x4000		# 16K kernel stack, quadword aligned
.comm stack, STACKSIZE, 32

loader:
	mov $(stack + STACKSIZE), %esp
	push %eax
	push %ebx

	call kmain

	cli
hang:				# if kmain returns
	hlt
	jmp hang

# gdt flush support for x86
# C signature: void gdt_flush( struct gdt * p );

.global gdt_flush
gdt_flush:
	mov 4(%esp), %eax
	lgdt (%eax)
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss
	jmp $0x8, $.flush	# far jump to reload cs
.flush:
	ret

