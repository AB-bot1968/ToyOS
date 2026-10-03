#!/usr/bin/env bash
set -euo pipefail
root="$(cd "$(dirname "$0")" && pwd)"
cd "$root"
grep -q 'FIX48B SAFE MODE POLICY EXPECTED-DENIAL TEST FIX' VERSION.txt
grep -q 'test_expect_safe_deny_rt' src/user_shell.c
grep -q 'EXPECT SAFE_DENY_RT' TST/TESTSFP.TST
grep -q 'START RT 1' TST/TESTSFP.TST
! grep -q '^KEY F10$' TST/TESTSFP.TST
grep -q 'TST/TESTSFP.TST' build.sh
printf '%s\n' 'FIX48B SAFE policy expected-denial test check: OK'
