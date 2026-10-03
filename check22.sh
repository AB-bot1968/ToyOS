#!/bin/sh
set -eu
fail(){ echo "CHECK FAILED: $1"; exit 1; }
grep -q '#define SYS_PORT_OUT8 *13u' src/kernel.c || fail syscall-13-kernel
grep -q '#define SYS_PORT_IN8 *14u' src/kernel.c || fail syscall-14-kernel
grep -q 'case SYS_PORT_OUT8' src/kernel.c || fail syscall-13-handler
grep -q 'case SYS_PORT_IN8' src/kernel.c || fail syscall-14-handler
grep -q 'outb((uint16_t)a,(uint8_t)b)' src/kernel.c || fail kernel-outb
 grep -q 'inb((uint16_t)a)' src/kernel.c || fail kernel-inb
grep -q 'sys_port_out8' src/user_shell.c || fail user-out-wrapper
grep -q 'sys_port_in8' src/user_shell.c || fail user-in-wrapper
grep -q 'prefix(line,"outb ")' src/user_shell.c || fail outb-command
grep -q 'prefix(line,"inb ")' src/user_shell.c || fail inb-command
grep -q '0x' src/user_shell.c || fail hex-parser
grep -q 'put("\\nsyscall 1 -> "' src/user_shell.c || fail syscall-output-newline
 grep -q '\[13\] SYS_PORT_OUT8 validation' src/user_shell.c || fail syscall-test-13
grep -q '\[14\] SYS_PORT_IN8 validation' src/user_shell.c || fail syscall-test-14
grep -q 'sys_port_out8' src/hello.c || fail exe-out-wrapper
grep -q 'sys_port_in8' src/hello.c || fail exe-in-wrapper
! grep -qE '__asm__[^[:cntrl:]]*(cli|sti|hlt|lgdt|lidt|ltr|outb|inb|mov[[:space:]]+[^[:cntrl:]]*cr[0-9])' src/user_shell.c || fail privileged-user-instruction
echo 'PASS: v22 arbitrary byte port I/O syscalls, shell commands, EXE wrappers and syscall output checks'
