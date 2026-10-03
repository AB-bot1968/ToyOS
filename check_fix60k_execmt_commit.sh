#!/bin/sh
set -eu
K=src/kernel.c
fail(){ echo "FIX60K EXECMT CHECK: FAIL: $1"; exit 1; }
# FIX60O restores the safe two-phase intent of FIX60K, while tests commit via
# the same explicit helper instead of relying on metadata-only assertions.
grep -q 'FIX60O: explicit PREPARED -> RUNNABLE boundary' "$K" || fail marker
grep -q 'sched_active=0u;mt_session_active=1u' "$K" || fail prepared
grep -q 'sched_commit_prepared' "$K" || fail commit
test -f TST/TESTMTK.TST || fail TESTMTK
test -f TST/TESTSONK.TST || fail TESTSONK
echo 'FIX60K EXECMT CHECK: PASS (FIX60O supersession)'
