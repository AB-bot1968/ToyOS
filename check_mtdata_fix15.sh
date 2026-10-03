#!/bin/sh
set -eu
K=src/kernel.c
S=src/user_shell.c
M=src/execmt_task.c
grep -q '#define SYS_MT_DATA        53u' "$K"
grep -q 'struct mt_data_snapshot' "$K"
grep -q 'mt_session_active' "$K"
grep -q 'case SYS_MT_DATA' "$K"
grep -q 'mt_data_clear_all();' "$K"
grep -q 'mtdata MT1..MT25' "$S"
grep -q 'mtdata_command' "$S"
grep -q 'SYS_MT_DATA      53u' "$M"
grep -q 'mt_result' "$M"
! find build -type f 2>/dev/null | grep -q .
echo 'PASS: FIX15 MTDATA session snapshots'
