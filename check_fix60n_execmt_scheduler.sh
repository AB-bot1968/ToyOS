#!/bin/sh
set -eu
K=src/kernel.c; U=src/user_shell.c; B=build.sh
fail(){ echo "FIX60N EXECMT/SCHEDULER CHECK: FAIL: $1" >&2; exit 1; }
# FIX60O supersedes FIX60N immediate activation after physical acceptance showed
# that IRQ0 could steal the foreground continuation before execmt printed success.
grep -q 'FIX60O: explicit PREPARED -> RUNNABLE boundary' "$K" || fail supersession
grep -q 'sched_commit_prepared' "$K" || fail commit
grep -q 'timestamp_us=rt_time_ticks\*10000u' "$K" || fail data_timestamp
if grep -q '^[[:space:]]*irq_enable(4);' "$K"; then fail irq4_unmasked; fi
grep -q 'MT_PROGRESS ' "$U" || fail progress_assert
grep -q 'for t in TST/\*.TST TST/ACCEPT.TXT' "$B" || fail "TST-staging"
for t in TST/TESTMTN.TST TST/TESTSONN.TST TST/TESTMTK.TST TST/TESTMTL.TST TST/TESTSONK.TST TST/TESTSONL.TST TST/TESTSONM.TST; do test -f "$t" || fail "$t"; grep -q "build/$t=$t" "$B" || fail "fat-$t"; done
grep -q 'memmove(q,q+1,7)' tools/sonarsim/sonarsim.c || fail sonarsim_resync
grep -q 'READY' tools/sonarsim/sonarsim.c || fail sonarsim_ready
echo 'FIX60N EXECMT/SCHEDULER CHECK: PASS (superseded by FIX60O boundary)'
