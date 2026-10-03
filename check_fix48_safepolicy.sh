#!/usr/bin/env bash
set -euo pipefail
need(){ grep -Fq "$2" "$1" || { echo "FIX48 check FAIL: $1 missing: $2"; exit 1; }; }
need src/kernel.c 'SYS_SAFE_POLICY        66u'
need src/kernel.c 'safe_deny_spawn++'
need src/kernel.c 'safe_deny_mt++'
need src/kernel.c 'safe_deny_rt++'
need src/user_shell.c 'safepolicy'
need src/user_shell.c 'SAFE_POLICY_FLAGS '
need TST/TESTSFP.TST 'ASSERT SAFE_DENY_TOTAL 3'
need TST/TESTSFP.TST 'ASSERT SAFE_POLICY_FLAGS 7'
need build.sh 'TST/TESTSFP.TST'
name=TESTSFP; [[ ${#name} -le 8 ]] || exit 1
echo 'FIX48 safe-mode policy static check: OK'
