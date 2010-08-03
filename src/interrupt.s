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

# isr stubs
.macro isr_noerrcode p
.global isr\p
isr\p:
	cli
	pushb #\p
.endm
