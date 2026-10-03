#!/bin/sh
set -eu
fail(){ echo "CHECK FAILED: $1"; exit 1; }
grep -q 'SYS_CONSOLE_READ is a character-read syscall' src/user_shell.c || fail read-contract-comment
grep -q 'r=sys_console_read(&input\[0\],1)' src/user_shell.c || fail read-first-char
grep -q 'r=sys_console_read(&input\[1\],1)' src/user_shell.c || fail read-second-char
grep -q 'r=sys_console_read(&input\[2\],1)' src/user_shell.c || fail read-enter
grep -Fq "input[2]=='\\n'" src/user_shell.c || fail read-enter-check
grep -q 'if(r==0xffffffffu){put("read=FAIL\\n");return;}' src/user_shell.c || fail read-error-path
grep -q 'case SYS_CONSOLE_READ' src/kernel.c || fail kernel-read-handler
grep -q 'sys_console_write("console-write OK\\n",17)' src/user_shell.c || fail write-test
grep -q 'case SYS_CONSOLE_WRITE' src/kernel.c || fail kernel-write-handler
echo 'PASS: v20 SYS_CONSOLE_READ test matches one-byte syscall contract'
