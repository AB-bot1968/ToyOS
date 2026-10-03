#!/bin/sh
set -eu
K=src/kernel.c; S=src/supervis.c; U=src/user_shell.c; B=build.sh; T=TST/TESTRST.TST
fail(){ echo "FIX51-RECOVERY CHECK: FAIL: $*"; exit 1; }
grep -q 'SYS_RECOVERY_MANAGER   68u' "$K" || fail 'syscall 68 changed/missing'
grep -q 'RECOVERY_RESTART_LIMIT 3u' "$K" || fail 'restart limit changed/missing'
grep -q 'RECOVERY_STATE_RECOVERED 4u' "$K" || fail 'RECOVERED state missing'
grep -q 'if(b==7u)' "$K" || fail 'extended query missing'
grep -q 'recovery_spawned' "$S" || fail 'spawn lifecycle report missing'
grep -q 'recovery_verified' "$S" || fail 'heartbeat verification report missing'
grep -q 'recovery_failed' "$S" || fail 'failure report missing'
grep -q 'RM_PID_CHANGED' "$U" || fail 'PID-change assertion missing'
grep -q 'RCONCE.EXE' "$B" || fail 'recovery worker not packaged'
grep -q 'TST/TESTRST.TST' "$B" || fail 'FIX51 test not packaged'
grep -q 'ASSERT RM_STATE 4' "$T" || fail 'success-state assertion missing'
grep -q 'ASSERT RM_PID_CHANGED' "$T" || fail 'new PID assertion missing'
grep -q 'ASSERT PROC_HANDLES 0' "$T" || fail 'resource cleanup assertion missing'
grep -q 'ASSERT RM_STATE 6' "$T" || fail 'SAFE escalation assertion missing'
grep -q 'FIX51 — managed process recovery lifecycle' TOYOS_ARCHITECTURE_VISION.md || fail 'Architecture Vision not updated'
echo 'FIX51-RECOVERY CHECK: PASS'
