#!/bin/sh
set -eu
fail(){ echo "FAIL: $1"; exit 1; }
B=build
[ -f "$B/kernel.pe" ] || B=build_audit

grep -q 'movl $0x200000,%esp' src/boot.S || fail "boot stack must be separate from TSS stack"
grep -q 'USER_STACK_TOP  0x003fffe0u' src/kernel.c || fail "user stack top must stay inside mapped page"
grep -q 'KERNEL_STACK_TOP 0x001f0000u' src/kernel.c || fail "TSS kernel stack address missing"
grep -q 'gdt_set(&gdt\[1\],0,0x3ffff,0x9a,0xc0)' src/kernel.c || fail "kernel code must be exactly 1 GiB"
grep -q 'gdt_set(&gdt\[2\],0,0x3ffff,0x92,0xc0)' src/kernel.c || fail "kernel data must be exactly 1 GiB"
grep -q 'gdt_set(&gdt\[3\],0,0x3ffff,0xfa,0xc0)' src/kernel.c || fail "user code must be exactly 1 GiB"
grep -q 'gdt_set(&gdt\[4\],0,0x3ffff,0xf2,0xc0)' src/kernel.c || fail "user data must be exactly 1 GiB"
grep -q 'tss.esp0=KERNEL_STACK_TOP' src/kernel.c || fail "TSS ESP0 missing"
grep -q 'tss.ss0=0x10u' src/kernel.c || fail "TSS SS0 missing"
grep -q 'pushl \$0x23;pushl \$0x003fffe0;pushfl;orl \$0x200,(%%esp);pushl \$0x1b;pushl \$_user_entry;iret' src/kernel.c || fail "CPL3 IRET frame/IF missing"
grep -q 'movw \$0x23,%ax' src/isr.S || fail "user data segment setup missing"
grep -q 'int \$0x80' src/user_shell.c || fail "user syscall wrapper missing"
grep -q 'struct frame{uint32_t edi,esi,ebp,oes,ebx,edx,ecx,eax;uint32_t gs,fs,es,ds;uint32_t int_no,error;uint32_t eip,cs,eflags;}' src/kernel.c || fail "ISR frame layout must match push-sequence"
grep -q 'ISR_ERR   8' src/isr.S || fail "double-fault vector must preserve CPU error code"
grep -q 'page_directory\[0\]=(uint32_t)page_table0|PAGE_P|PAGE_RW|PAGE_US;' src/kernel.c || fail "PDE[0] must allow user page-table walk"
grep -q 'outb(PIC1+1,0xff)' src/kernel.c || fail "PIC master must be masked before remap"
grep -q 'outb(PIC2+1,0xff)' src/kernel.c || fail "PIC slave must be masked before remap"
grep -q 'outb(PIC1+1,0x20)' src/kernel.c || fail "PIC master must remap IRQ0..7 to INT 32..39"
grep -q 'outb(PIC2+1,0x28)' src/kernel.c || fail "PIC slave must remap IRQ8..15 to INT 40..47"
grep -q 'outb(PIC1+1,0x04)' src/kernel.c || fail "PIC cascade ICW3 missing"
grep -q 'outb(PIC2+1,0x02)' src/kernel.c || fail "PIC slave ICW3 missing"
grep -q 'outb(PIC1+1,0x01)' src/kernel.c || fail "PIC master ICW4 missing"
grep -q 'outb(PIC2+1,0x01)' src/kernel.c || fail "PIC slave ICW4 missing"
grep -q 'outb(PIC1+1,0xfc)' src/kernel.c || fail "PIC final master mask missing"
grep -q 'outb(PIC2+1,0xff)' src/kernel.c || fail "PIC final slave mask missing"
grep -q 'orl $0x200,(%%esp)' src/kernel.c || fail "user IRET frame must set IF without CPL3 STI"
! grep -q '^    sti' src/isr.S || fail "CPL3 must not execute privileged STI"
! grep -q '^    cli' src/isr.S || fail "CPL3 must not execute privileged CLI"
! grep -q '^    hlt' src/isr.S || fail "CPL3 must not execute privileged HLT"
grep -q 'if(n==14){uint32_t cr2' src/kernel.c || fail "page-fault CR2 diagnostic missing"
grep -q 'page_directory\[0\]=(uint32_t)page_table0|PAGE_P|PAGE_RW|PAGE_US;' src/kernel.c || fail "PDE[0] must allow user page-table walk"
grep -q 'case SYS_DISK_READ:if((f->cs&3u)&&(c>8u' src/kernel.c || fail "user disk-read sector overflow guard missing"
grep -q 'case SYS_DISK_WRITE:if((f->cs&3u)&&(c>8u' src/kernel.c || fail "user disk-write sector overflow guard missing"
! grep -R -qE '\b(movaps|movups|movdqa|movdqu|xorps|xorpd|addps|subps|mulps|divps|pxor|pcmp|fld|fst|fadd|fsub|fmul|fdiv)\b' $B/kernel.dis 2>/dev/null || fail "SIMD/FPU instruction detected"
objdump -d $B/kernel.pe 2>/dev/null | grep -q 'iret' || fail "IRET missing in kernel binary"
grep -q 'static void console_scroll(void)' src/kernel.c || fail "console scrolling routine missing"
grep -q 'console_scroll();vga_y=H-1u' src/kernel.c || fail "console must scroll at bottom instead of wrapping to row 0"
grep -q 'static void console_newline(void)' src/kernel.c || fail "console newline routine missing"
echo "PASS: Ring-3 frame/GDT/TSS/paging/syscall static checks"
python3 - <<'PY'
from pathlib import Path
s=Path('src/kernel.c').read_text()
assert 'VGA_CRTC 0x3d4u' in s
assert 'outb(VGA_CRTC,0x0f)' in s and 'outb(VGA_CRTC,0x0e)' in s
assert 'static const char keymap[128]="\\0\\0331234567890-=\\b\\tqwertyuiop[]\\n' in s
assert 'static const char keymap_shift[128]="\\0\\033!@#$%^&*()_+\\b\\tQWERTYUIOP{}\\n' in s
print('PASS: VGA hardware cursor and Set-1 keyboard map')
PY
