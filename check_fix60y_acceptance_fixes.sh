#!/usr/bin/env bash
set -euo pipefail
fail(){ echo "FIX60Y CHECK FAIL: $*" >&2; exit 1; }
grep -q 'FIX60Y: foreground diagnostic yield is also a scheduler service point' src/kernel.c || fail 'RT yield service fix missing'
grep -q 'FIX60Y: wait for observable completed-job progress' src/user_shell.c || fail 'RT progress wait fix missing'
grep -q 'Commit heartbeat first' src/sonardrv.c || fail 'SONARTST heartbeat ordering fix missing'
grep -q 'Pace the deterministic producer at 25 Hz' src/sonarpub.c || fail 'SONARPUB pacing fix missing'
[ "$(find TST -maxdepth 1 -name 'TEST*.TST' | wc -l)" -eq 43 ] || fail 'expected 43 TEST*.TST'
! find TST -maxdepth 1 -name 'B*.TST' | grep -q . || fail 'batch variants returned'
! grep -q 'test_log_buffer\|test_log_flush\|test_log_write' src/user_shell.c || fail 'test log implementation returned'
echo 'FIX60Y ACCEPTANCE FIXES: PASS'
