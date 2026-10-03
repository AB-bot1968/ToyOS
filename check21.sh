#!/bin/sh
set -eu
fail(){ echo "CHECK FAILED: $1"; exit 1; }
grep -q 'case SYS_CONSOLE_READ' src/kernel.c || fail kernel-read-handler
grep -q 'sti;hlt;cli' src/kernel.c || fail kernel-blocking-read
grep -Fq 'r=sys_console_read(&input[0],1);' src/user_shell.c || fail test-read-1
grep -Fq 'r=sys_console_read(&input[1],1);' src/user_shell.c || fail test-read-2
grep -Fq 'r=sys_console_read(&input[2],1);' src/user_shell.c || fail test-read-3
grep -Fq "if(input[0]=='O'&&input[1]=='K'&&input[2]=='\\n')" src/user_shell.c || fail test-validation
grep -q 'sys_console_read(&x,1)' src/user_shell.c || fail syscall-demo-read
! grep -qE '__asm__[^[:cntrl:]]*(cli|sti|hlt|lgdt|lidt|ltr|outb|inb|mov[[:space:]]+[^[:cntrl:]]*cr[0-9])' src/user_shell.c || fail privileged-user-instruction
echo 'PASS: v21 console-read test and Ring-0 blocking path checks'
