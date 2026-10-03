#!/bin/sh
set -eu
cd "$(dirname "$0")"
grep -q '#define SYS_EXEC_ARGS   25u' src/kernel.c
grep -q '#define SYS_EXEC_QUEUE_ARGS 26u' src/kernel.c
grep -q 'case SYS_EXEC_ARGS' src/kernel.c
grep -q 'case SYS_EXEC_QUEUE_ARGS' src/kernel.c
grep -q 'COMDRV.EXE' build.sh
[ -f src/comdrv.c ]
grep -q 'SYS_PORT_OUT8' src/comdrv.c
grep -q 'SYS_PORT_IN8' src/comdrv.c
if grep -Eq '\b(inb|outb|inw|outw|inl|outl)\s*\(' src/comdrv.c; then
  echo 'ERROR: direct port instruction wrapper found in COMDRV source' >&2
  exit 1
fi
grep -q '115200' src/comdrv.c
grep -q '8N1' src/comdrv.c
grep -q 'SYS_FILE_OPEN' src/comdrv.c
grep -q 'SYS_FILE_READ' src/comdrv.c
grep -q 'SYS_FILE_WRITE' src/comdrv.c
grep -q 'SYS_FILE_CLOSE' src/comdrv.c
grep -q 'exec COMDRV.EXE PORT SEND|RECV FILE' src/user_shell.c
grep -q 'execq REPEAT COMDRV.EXE PORT SEND|RECV FILE' src/user_shell.c
sh -n build.sh
echo 'check51: PASS'
