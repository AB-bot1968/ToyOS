#!/bin/sh
set -eu
f=src/execmt_task.c
if grep -Eq 'SYS_CONSOLE|CONSOLE_WRITE|sys_console' "$f"; then
  echo 'FAIL: EXECMT task still references console syscall'; exit 1
fi
grep -q 'SYS_MT_DATA' "$f"
grep -q 'mt_publish' "$f"
echo 'PASS: FIX16 EXECMT tasks use MTDATA only'
