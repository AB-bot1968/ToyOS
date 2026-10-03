#!/bin/sh
set -eu
U=src/user_shell.c
C=src/comdrv.c
fail(){ echo "FAIL: $1" >&2; exit 1; }
grep -q 'exec_com_command(line+16)' "$U" || fail 'direct COM exec offset'
grep -q 'exec_queue_com_command(line+14,1)' "$U" || fail 'execq forever COM offset'
grep -q 'exec_queue_com_command(start,0);return;' "$U" || fail 'numeric execq COM dispatch'
grep -q 'exec COMDRV.EXE PORT SEND|RECV FILE' "$U" || fail 'direct COM help'
grep -q 'execq REPEAT COMDRV.EXE PORT SEND|RECV FILE' "$U" || fail 'finite COM queue help'
grep -q 'execq forever COMDRV.EXE PORT SEND|RECV FILE' "$U" || fail 'forever COM queue help'
grep -q '"=b"(port)' "$C" || fail 'EBX ABI constraint'
grep -q '"=c"(dir)' "$C" || fail 'ECX ABI constraint'
grep -q '"=d"(file)' "$C" || fail 'EDX ABI constraint'
grep -q 'parse_port' "$C" || fail 'strict COM port parser'
if gcc -m32 -ffreestanding -fsyntax-only -fno-pie -fno-stack-protector -nostdinc src/user_shell.c; then :; else fail 'user_shell syntax'; fi
if gcc -m32 -ffreestanding -fsyntax-only -fno-pie -fno-stack-protector -nostdinc src/comdrv.c; then :; else fail 'comdrv syntax'; fi
echo 'CHECK55 PASS'
