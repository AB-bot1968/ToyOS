#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
K="$ROOT/src/kernel.c"
T="$ROOT/TST/TESTLGR.TST"
grep -q 'FIX60ZEA: normal exit of the final MT process' "$K"
grep -q 'mt_session_active=0u;mt_session_count=0u' "$K"
grep -q 'ASSERT MT_ACTIVE 0' "$T"
grep -q '^RUN exec SONARVWR.EXE$' "$T"
grep -q '^RUN execmt SONARPUB.EXE SONARLOG.EXE$' "$T"
[ "$(grep -ve '^[[:space:]]*$' "$ROOT/TST/ACCEPT.TXT" | sort -u | wc -l)" -eq 45 ]
while IFS= read -r n; do [ -z "$n" ] && continue; [ -f "$ROOT/TST/$n" ] || { echo "missing $n"; exit 1; }; done < "$ROOT/TST/ACCEPT.TXT"
! find "$ROOT" -type f -name 'TST.LOG' | grep -q .
! find "$ROOT/TST" -maxdepth 1 -type f -name 'B*.TST' | grep -q .
echo 'FIX60ZEA CHECK PASS: final MT exit closes empty session; 45 acceptance tests present'
