#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")" && pwd)"
cd "$ROOT"
[ "$(find TST -maxdepth 1 -type f -name 'TEST*.TST' | wc -l)" -eq 43 ]
! find TST -maxdepth 1 -type f -name 'B*.TST' | grep -q .
[ "$(grep -cv '^[[:space:]]*\(#\|$\)' TST/ACCEPT.TXT)" -eq 43 ]
! grep -qE 'test_log_buffer|TEST_LOG_BUFFER|TST\.LOG' src/user_shell.c
! grep -qE 'TST/B[A-Z0-9]+\.TST|TST/TESTTST\.TST' build.sh
grep -q 'acceptance started (console only)' src/user_shell.c
grep -q 'TEST RESULT: PASS=' src/user_shell.c
grep -q 'FAILED TESTS / LINES:' src/user_shell.c
grep -q 'test_summary_record(path,lineno)' src/user_shell.c
grep -q 'TESTS: ' src/user_shell.c
echo 'FIX60X TEST CONSOLE ROLLBACK: PASS'
