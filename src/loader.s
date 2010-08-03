.global loader
.global halt

.set ALIGN,			1<<0
.set MEMINFO,		1<<1
.set FLAGS,			ALIGN | MEMINFO
.set MAGIC,			0x1badb002
.set CHECKSUM,		-(MAGIC + FLAGS)

.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.set STACKSIZE, 0x4000		# 16K kernel stack, quadword aligned
.comm stack, STACKSIZE, 32

loader:
	mov $(stack + STACKSIZE), %esp
	push %ebx
	push %eax

	call kmain

	cli
hang:				# if kmain returns
	hlt
	jmp hang

