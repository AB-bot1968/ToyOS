#!/bin/sh
set -eu
f=src/user_shell.c
grep -q 'exec_com_command(line+16)' "$f"
grep -q 'exec_queue_com_command(line+14,1)' "$f"
grep -q 'exec_queue_com_command(start,0);return;' "$f"
grep -q 'prefix(p,"COMDRV.EXE ")' "$f"
echo "check53: PASS"
