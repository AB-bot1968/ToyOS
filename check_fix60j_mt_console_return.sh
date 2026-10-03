#!/bin/sh
set -eu
K=src/kernel.c
S=src/sonardrv.c
fail(){ echo "FIX60J/60M MT CONSOLE RETURN CHECK: FAIL: $1"; exit 1; }
# ABI remains for compatibility, but FIX60M forbids SONARDRV from using the
# direct saved-shell restore path after a successful sensor transaction.
grep -q '#define SYS_MT_YIELD            74u' "$K" || fail abi
grep -q 'static void sched_yield' "$K" || fail kernel-yield
grep -q 'case SYS_MT_YIELD:if(f->cs&3u)sched_yield(f)' "$K" || fail dispatch
! grep -q 'sc(SYS_MT_YIELD' "$S" || fail sonar-must-use-pit-only
! grep -q 'SYS_CONSOLE_WRITE' "$S" || fail background-console
grep -q 'SYS_PROCESS_HEARTBEAT' "$S" || fail heartbeat
grep -q 'FIX60M' "$S" || fail fix60m-marker
echo 'FIX60J/60M MT CONSOLE RETURN CHECK: PASS'
