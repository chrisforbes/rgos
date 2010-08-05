; gdt flush support for x86
; C signature: void gdt_flush( struct gdt_ptr * p );

[bits 32]
[section .text]

[global gdt_flush]
gdt_flush:
	mov eax, [esp+4]
	lgdt [eax]
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	jmp 0x8:.flush
.flush:
	ret

; idt flush support for x86
; C signature: void idt_flush( struct idt_ptr * p );

[global idt_flush]
idt_flush:
	mov eax, [esp+4]
	lidt [eax]
	ret

; isr stub support
; C signature: N/A

%macro isr_noerrcode 1
[global isr%1]
isr%1:
	cli
	push 0
	push %1
	jmp isr_common_stub
%endmacro

%macro isr_errcode 1
[global isr%1]
isr%1:
	cli
	push %1
	jmp isr_common_stub
%endmacro

%macro irq 2
[global irq%1]
irq%1:
	cli
	push 0
	push %2
	jmp irq_common_stub
%endmacro

isr_noerrcode 0
isr_noerrcode 1
isr_noerrcode 2
isr_noerrcode 3
isr_noerrcode 4
isr_noerrcode 5
isr_noerrcode 6
isr_noerrcode 7
isr_errcode 8
isr_noerrcode 9
isr_errcode 10
isr_errcode 11
isr_errcode 12
isr_errcode 13
isr_errcode 14
isr_noerrcode 15
isr_noerrcode 16
isr_noerrcode 17
isr_noerrcode 18
isr_noerrcode 19
isr_noerrcode 20
isr_noerrcode 21
isr_noerrcode 22
isr_noerrcode 23
isr_noerrcode 24
isr_noerrcode 25
isr_noerrcode 26
isr_noerrcode 27
isr_noerrcode 28
isr_noerrcode 29
isr_noerrcode 30
isr_noerrcode 31

irq 0, 32
irq 1, 33
irq 2, 34
irq 3, 35
irq 4, 36
irq 5, 37
irq 6, 38
irq 7, 39
irq 8, 40
irq 9, 41
irq 10, 42
irq 11, 43
irq 12, 44
irq 13, 45
irq 14, 46
irq 15, 47

%macro gen_common_stub 2
[extern %2]
%1:
	pusha
	mov ax, ds
	push eax			; save the data segment selector
	
	mov ax, 0x10		; load all (data) segment registers
	mov ds, ax			; with hard-coded kernel data segment.
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	call %2				; generic handler in C
	
	pop eax				; restore original data segment selector
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	popa
	add esp, 8
	sti
	iret				; pop cs, eip, eflags, ss, esp
%endmacro

gen_common_stub isr_common_stub, isr_handler
gen_common_stub irq_common_stub, irq_handler
	
	
	
