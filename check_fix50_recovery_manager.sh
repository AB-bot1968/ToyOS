#!/bin/sh
set -eu
K=src/kernel.c; S=src/supervis.c; U=src/user_shell.c
fail(){ echo "FIX50-RM CHECK: FAIL: $*"; exit 1; }
grep -q 'SYS_RECOVERY_MANAGER   68u' "$K" || fail 'syscall 68 missing'
grep -q 'RECOVERY_RESTART_LIMIT 3u' "$K" || fail 'restart limit missing'
grep -q 'case SYS_RECOVERY_MANAGER' "$K" || fail 'kernel manager missing'
grep -q 'recovery_detect' "$S" || fail 'supervisor does not delegate'
grep -q 'RM_ACTION' "$U" || fail 'test observability missing'
grep -q 'TST/TESTRCM.TST' build.sh || fail 'TESTRCM not in image'
echo 'FIX50-RM CHECK: PASS'
