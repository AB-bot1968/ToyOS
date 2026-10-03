#!/bin/sh
set -eu
f=src/user_shell.c
grep -q 'BIN/NETDRV.EXE' "$f"
grep -q 'exec NETDRV.EXE: FAIL' "$f"
grep -q 'r=sys_exec_args(bin_name,args)' "$f"
grep -q 'r=sys_exec_args(name,args)' "$f"
echo 'check114: PASS'
