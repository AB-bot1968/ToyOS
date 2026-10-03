#!/bin/sh
set -eu
K=src/kernel.c; U=src/user_shell.c; S=src/sonardrv.c; SIM=tools/sonarsim/sonarsim.c; B=build.sh
fail(){ echo "FIX60O EXECMT/SERIAL BOUNDARY CHECK: FAIL: $1" >&2; exit 1; }
grep -q 'FIX60O: explicit PREPARED -> RUNNABLE boundary' "$K" || fail prepared_marker
grep -q 'sched_active=0u;mt_session_active=1u' "$K" || fail prepared_state
grep -q 'sched_commit_prepared' "$K" || fail commit_helper
grep -q 'SYS_EXECMT_COMMIT.*75u' "$K" || fail commit_syscall
grep -q 'if(mt_session_active&&!sched_active&&!exe_active)(void)sched_commit_prepared' "$K" || fail shell_commit
grep -q 'syscall3(SYS_EXECMT_COMMIT' "$U" || fail test_commit
grep -q 'SONAR_TEST_INJECT' "$S" || fail success_inject
grep -q 'MT_HEARTBEAT ' "$U" || fail heartbeat_assert
grep -q 'DATA_PUBLISHED ' "$U" || fail data_assert
grep -q 'FILE_FLAG_OVERLAPPED' "$SIM" || fail overlapped
grep -q 'WaitForSingleObject' "$SIM" || fail bounded_wait
grep -q 'tx=TIMEOUT' "$SIM" || fail tx_timeout_diag
grep -q 'tx=%u' "$SIM" || fail tx_success_diag
for t in TST/TESTSONO.TST SONARTST.EXE; do grep -q "$t" "$B" || fail "pack-$t"; done
grep -q './check_fix60o_execmt_serial_boundary.sh' "$B" || fail checker_chain
grep -q 'FIX60O — явный EXECMT commit и bounded host serial boundary' TOYOS_ARCHITECTURE_VISION.md || fail architecture
test -f VERIFICATION_V67_11_B_FIX60O_RU.md || fail verification
test -f RELEASE_NOTES_V67_11_B_FIX60O_RU.md || fail release_notes
if grep -q '^[[:space:]]*irq_enable(4);' "$K"; then fail irq4_unmasked; fi
echo 'FIX60O EXECMT/SERIAL BOUNDARY CHECK: PASS'
