# gdt flush support for x86
# C signature: void gdt_flush( struct gdt_ptr * p );

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

# idt flush support for x86
# C signature: void idt_flush( struct idt_ptr * p );

.global idt_flush
idt_flush:
	mov 4(%esp), %eax
	lidt (%eax)
	ret

# isr stub support
# C signature: N/A

.macro isr_noerrcode p
.global isr\p
isr\p:
	cli
	push 0
	push \p
	jmp isr_common_stub
.endm

.macro isr_errcode p
.global isr\p
isr\p:
	cli
	push \p
	jmp isr_common_stub
.endm

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

.extern isr_handler

isr_common_stub:
	pusha
	mov %ds, %ax
	push %eax			# save the data segment selector
	
	mov $0x10, %ax		# load all (data) segment registers
	mov %ax, %ds		# with hard-coded kernel data segment.
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	
	call isr_handler
	
	pop %eax			# restore original data segment selector
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	
	popa
	add $0x8, %esp
	sti
	iret				# pop cs, eip, eflags, ss, esp
	
	
