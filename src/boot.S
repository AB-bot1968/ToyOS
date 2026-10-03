.code16
.section .text
.global _start
_start:
    cli
    xorw %ax,%ax
    movw %ax,%ds
    movw %ax,%es
    movw %ax,%ss
    movw $0x7c00,%sp
    movb %dl, boot_drive

    call enable_a20
    call detect_memory
    call load_kernel

    cli
    lgdt gdt_desc
    movl %cr0,%eax
    orl $1,%eax
    movl %eax,%cr0
    ljmp $0x08,$pm_entry

hang:
    cli
    hlt
    jmp hang

enable_a20:
    inb $0x92,%al
    orb $0x02,%al
    andb $0xfe,%al
    outb %al,$0x92
    ret

# E820: sum usable (type 1) RAM, capped at 1 GiB.
# The value is stored outside the visible 80x25 VGA text buffer, at physical 0xB8FF0.
detect_memory:
    xorl %eax,%eax
    movl %eax,memory_total
    xorl %ebx,%ebx
    movw $0x7000,%di
    xorw %ax,%ax
    movw %ax,%es
.e820:
    movl $0xE820,%eax
    movl $0x534D4150,%edx
    movl $24,%ecx
    int $0x15
    jc .fallback
    cmpl $0x534D4150,%eax
    jne .fallback
    cmpl $1,16(%di)
    jne .next
    cmpl $0,4(%di)
    jne .next
    cmpl $0x40000000,0(%di)
    jae .next

    # Saturating add: total = min(total + length, 1 GiB).
    movl memory_total,%ecx
    cmpl $0x40000000,%ecx
    jae .done
    cmpl $0x40000000,%eax
    jae .cap
    movl %eax,%edx
    negl %edx
    cmpl %edx,%ecx
    jae .cap
    addl %eax,%ecx
    movl %ecx,memory_total
.next:
    cmpl $0,%ebx
    jne .e820
.done:
    call store_memory
    ret
.cap:
    movl $0x40000000,memory_total
    jmp .done

# E801 fallback.
.fallback:
    movw $0xE801,%ax
    int $0x15
    jc .zero
    # AX: KiB from 1 MiB through 16 MiB.
    movzwl %ax,%eax
    shll $10,%eax
    addl $0x00100000,%eax
    # BX: 64-KiB blocks above 16 MiB.
    movzwl %bx,%ecx
    shll $16,%ecx
    addl %ecx,%eax
    cmpl $0x40000000,%eax
    jb .fallback_store
    movl $0x40000000,%eax
.fallback_store:
    movl %eax,memory_total
    call store_memory
    ret
.zero:
    xorl %eax,%eax
    movl %eax,memory_total
    call store_memory
    ret

store_memory:
    movw $0xB800,%ax
    movw %ax,%es
    movl memory_total,%eax
    movl %eax,%es:0x0ff0
    xorw %ax,%ax
    movw %ax,%es
    ret

# FIX53: LBA1 contains the layout descriptor plus its compact loader.
# The boot sector knows only this one fixed metadata sector.  The layout loader
# validates the descriptor and loads the variable-size kernel from its declared LBA.
load_kernel:
    movw $layout_dap,%si
    movb boot_drive,%dl
    movb $0x42,%ah
    int $0x13
    jc disk_fail
    movb boot_drive,%dl
    lcall $0x0000,$0x0640
    ret

disk_fail:
    movw $0xB800,%ax
    movw %ax,%es
    movb $'D',%es:0
    movb $0x4f,%es:1
    jmp hang

.align 4
memory_total: .long 0
boot_drive:   .byte 0
.align 4
layout_dap:
    .byte 0x10,0
    .word 1
    .word 0x0600
    .word 0x0000
    .long 1
    .long 0

.align 8
gdt:
    .word 0,0,0,0
    # code: base 0, limit 0x03FFFF, G=1 => exactly 1 GiB
    .word 0xffff,0
    .byte 0,0x9a,0xc3,0
    # data: base 0, limit 0x03FFFF, G=1 => exactly 1 GiB
    .word 0xffff,0
    .byte 0,0x92,0xc3,0
gdt_desc:
    .word gdt_desc-gdt-1
    .long gdt

.code32
pm_entry:
    movw $0x10,%ax
    movw %ax,%ds
    movw %ax,%es
    movw %ax,%ss
    movw %ax,%fs
    movw %ax,%gs
    movl $0x200000,%esp
    ljmp $0x08,$0x7e00

.org 510
.word 0xaa55
