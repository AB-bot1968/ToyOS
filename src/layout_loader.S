.code16
.section .text
# FIX53 layout sector. Bytes 0..63 are overwritten by tools/mklayout with
# the descriptor. Code begins at physical 0000:0640 and remains resident while
# the kernel is loaded to 0x7e00.
.org 64
.global layout_loader_entry
layout_loader_entry:
    movb %dl,stage_boot_drive
    # Validate LAY1 header and checksum (sum of 16 DWORDs == 0).
    cmpl $0x3159414c,0x0600
    jne layout_fail
    cmpl $1,0x0604
    jne layout_fail
    cmpl $64,0x0608
    jne layout_fail
    xorl %eax,%eax
    movw $0x0600,%si
    movw $16,%cx
1:  addl (%si),%eax
    addw $4,%si
    loop 1b
    testl %eax,%eax
    jne layout_fail

    movl 0x0610,%eax              # kernel_lba
    cmpl $2,%eax
    jb layout_fail
    movl %eax,kernel_lba_cur
    movl 0x0618,%eax              # kernel_sectors
    testl %eax,%eax
    jz layout_fail
    cmpl $0xffff,%eax
    ja layout_fail
    movw %ax,kernel_left
    movl 0x0638,%edi              # load_addr
    cmpl $0x7e00,%edi
    jne layout_fail

    # Disk-loaded image must remain below declared memory limit.
    movzwl kernel_left,%eax
    shll $9,%eax
    addl %edi,%eax
    jc layout_fail
    cmpl 0x063c,%eax              # load_limit
    ja layout_fail

load_loop:
    movzwl kernel_left,%eax
    testl %eax,%eax
    jz load_done
    cmpl $64,%eax
    jbe 2f
    movl $64,%eax
2:  movw %ax,kernel_dap+2

    # Normalize segment:offset before every <=64-sector transfer.
    movl %edi,%edx
    andl $15,%edx
    movw %dx,kernel_dap+4
    movl %edi,%edx
    shrl $4,%edx
    movw %dx,kernel_dap+6
    movl kernel_lba_cur,%edx
    movl %edx,kernel_dap+8
    movl $0,kernel_dap+12

    movb stage_boot_drive,%dl
    movw $kernel_dap,%si
    movb $0x42,%ah
    int $0x13
    jc layout_fail

    movzwl kernel_dap+2,%eax
    subw %ax,kernel_left
    addl %eax,kernel_lba_cur
    shll $9,%eax
    addl %eax,%edi
    jmp load_loop

load_done:
    lret

layout_fail:
    movw $0xb800,%ax
    movw %ax,%es
    movb $'L',%es:2
    movb $0x4f,%es:3
3:  cli
    hlt
    jmp 3b

.align 4
kernel_lba_cur: .long 0
kernel_left: .word 0
stage_boot_drive: .byte 0
.align 4
kernel_dap:
    .byte 0x10,0
    .word 0
    .word 0
    .word 0
    .long 0
    .long 0

.org 512
