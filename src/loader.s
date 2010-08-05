[bits 32]
[GLOBAL start]
[extern kmain]

MULTIBOOT_HEADER_MAGIC equ 0x1badb002
MULTIBOOT_HEADER_FLAGS equ 3
MULTIBOOT_HEADER_CHECKSUM equ -( MULTIBOOT_HEADER_FLAGS + MULTIBOOT_HEADER_MAGIC )

[section .text]
ALIGN 4
multiboot_header:
	dd MULTIBOOT_HEADER_MAGIC
	dd MULTIBOOT_HEADER_FLAGS
	dd MULTIBOOT_HEADER_CHECKSUM
	
start:
	lgdt [trickgdt]
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	
	jmp 0x8:higherhalf

higherhalf:
	mov esp, sys_stack
	call kmain
	jmp $

[section .setup]
trickgdt:
	dw gdt_end - gdt - 1
	dd gdt
gdt:
	dd 0, 0
	db 0xFF, 0xFF, 0, 0, 0, 10011010b, 11001111b, 0x40
	db 0xFF, 0xFF, 0, 0, 0, 10010010b, 11001111b, 0x40
gdt_end:

[section .bss]
resb 0x1000
sys_stack:

