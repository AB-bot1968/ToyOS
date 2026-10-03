#!/bin/sh
set -eu
fail(){ echo "CHECK FAILED: $1"; exit 1; }
grep -q '#define KBD_STATUS 0x64u' src/kernel.c || fail kbd-status
grep -q 'keyboard_init' src/kernel.c || fail keyboard-init
grep -q 'outb(KBD_STATUS,0xae)' src/kernel.c || fail keyboard-enable
grep -q 'if(!(st&1u))return' src/kernel.c || fail irq-status-check
grep -q 'case SYS_CONSOLE_WRITE' src/kernel.c || fail write-handler
grep -q 'case SYS_CONSOLE_READ' src/kernel.c || fail read-handler
grep -q 'sys_console_write("console-write OK\\n",17)' src/user_shell.c || fail write-test
grep -q 'sys_console_read(&c,1)' src/user_shell.c || fail read-wrapper
# User mode must not contain privileged instructions.
! grep -qE '__asm__[^[:cntrl:]]*(cli|sti|hlt|lgdt|lidt|ltr|outb|inb|mov[[:space:]]+[^[:cntrl:]]*cr[0-9])' src/user_shell.c || fail privileged-user-instruction
# Verify the two public wrappers use INT 80h ABI.
grep -q 'int \$0x80' src/user_shell.c || fail int80
# Ensure read blocks only in kernel and explicitly wakes on IRQ1.
grep -q 'sti;hlt;cli' src/kernel.c || fail blocking-read
grep -q 'else if(n==33)keyboard_irq()' src/kernel.c || fail irq1-dispatch
grep -q 'irq_enable(1)' src/kernel.c || fail irq1-enable
echo 'PASS: v19 console read/write and keyboard path static checks'
