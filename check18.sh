#!/bin/sh
set -eu
fail(){ echo "CHECK FAILED: $1"; exit 1; }
grep -q 'case SYS_CONSOLE_READ' src/kernel.c || fail syscall-handler
grep -q 'if(b==0u){f->eax=0;break;}' src/kernel.c || fail zero-length-read
grep -q 'sti;hlt;cli' src/kernel.c || fail blocking-read
grep -q 'sys_console_read(&c,1)' src/user_shell.c || fail readline-call
grep -q 'r==0xffffffffu' src/user_shell.c || fail read-error-check
grep -q 'SYS_CONSOLE_READ: type OK then Enter' src/user_shell.c || fail interactive-test
! grep -qE '__asm__[^[:cntrl:]]*(cli|sti|hlt|lgdt|lidt|ltr|outb|inb|mov[[:space:]]+[^[:cntrl:]]*cr[0-9])' src/user_shell.c || fail privileged-user-instruction
echo "PASS: SYS_CONSOLE_READ blocking/error-path checks"
