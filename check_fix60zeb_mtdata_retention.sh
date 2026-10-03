#!/bin/sh
set -eu
K=src/kernel.c
T=TST/TESTKEY.TST
L=TST/TESTLGR.TST
grep -q 'FIX60ZEB: natural exit closes the scheduler session but retains' "$K"
# Final natural EXIT block must close session without clearing MTDATA.
python3 - <<'PY'
from pathlib import Path
s=Path('src/kernel.c').read_text()
a=s.index('/* FIX60ZEA: normal exit of the final MT process')
b=s.index('}else if(sched_pick_any_ready()<0)',a)
blk=s[a:b]
assert 'mt_session_active=0u' in blk
assert 'mt_session_count=0u' in blk
assert 'mt_data_clear_all();' not in blk
# New-session paths must still clear retained data.
assert 'if(!mt_session_active){\n        mt_data_clear_all();' in s
assert 'static int execmt_prepare' in s and 'mt_data_clear_all();' in s[s.index('static int execmt_prepare'):s.index('static void sched_start')]
PY
grep -q 'ASSERT LAST_MTDATA ARGV:ARGVDIAG.EXE|ONE|TWO|THREE' "$T"
grep -q 'ASSERT MT_ACTIVE 0' "$L"
echo 'FIX60ZEB CHECK PASS: final MT EXIT closes session and retains MTDATA'
