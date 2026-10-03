.code32
.section .text,"ax"

.global _zero_bss
_zero_bss:
    pushl %edi
    cld
    movl $___bss_start,%edi
    movl $___bss_end,%ecx
    subl %edi,%ecx
    xorl %eax,%eax
    rep
stosb
    popl %edi
    ret

.global _isr_common
_isr_common:
    cld
    pushl %ds
    pushl %es
    pushl %fs
    pushl %gs
    pushal
    movw $0x10,%ax
    movw %ax,%ds
    movw %ax,%es
    pushl %esp
    call _interrupt_dispatch
    addl $4,%esp
    popal
    popl %gs
    popl %fs
    popl %es
    popl %ds
    addl $8,%esp
    iret

.section .usertext,"ax"
.global _user_entry
_user_entry:
    cld
    movw $0x23,%ax
    movw %ax,%ds
    movw %ax,%es
    movw %ax,%fs
    movw %ax,%gs
    /* IF is supplied by the IRET frame. STI is privileged at CPL3 when IOPL=0. */
    call _shell_run
1:
    jmp 1b

.section .text,"ax"

.macro ISR_NOERR n
.global _isr\n
_isr\n:
    pushl $0
    pushl $\n
    jmp _isr_common
.endm

.macro ISR_ERR n
.global _isr\n
_isr\n:
    pushl $\n
    jmp _isr_common
.endm

ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR   8
ISR_NOERR 9
ISR_ERR   10
ISR_ERR   11
ISR_ERR   12
ISR_ERR   13
ISR_ERR   14
ISR_NOERR 15
ISR_NOERR 16
ISR_ERR   17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_NOERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_NOERR 30
ISR_NOERR 31
ISR_NOERR 32
ISR_NOERR 33
ISR_NOERR 34
ISR_NOERR 35
ISR_NOERR 36
ISR_NOERR 37
ISR_NOERR 38
ISR_NOERR 39
ISR_NOERR 40
ISR_NOERR 41
ISR_NOERR 42
ISR_NOERR 43
ISR_NOERR 44
ISR_NOERR 45
ISR_NOERR 46
ISR_NOERR 47
ISR_NOERR 128
