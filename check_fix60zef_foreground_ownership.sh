#!/bin/sh
set -eu
ROOT=`CDPATH= cd -- "$(dirname -- "$0")" && pwd`
cd "$ROOT"
python3 - <<'PY'
from pathlib import Path
s=Path('src/kernel.c').read_text()
# Console read must be a pure ownership/idle syscall: no address-space switch.
a=s.index('case SYS_CONSOLE_READ:')
b=s.index('case SYS_CONSOLE_POLL:',a)
blk=s[a:b]
assert 'rt_switch_to(' not in blk
assert 'load_cr3(' not in blk
assert 'rt_shell_waiting=1u' in blk
assert 'f->eax=2u' in blk
# MT slack is explicitly bounded and ordinary foreground commands do not donate implicitly.
assert 'static volatile uint32_t rt_mt_slack_active;' in s
assert 'contract is one PIT interval maximum' in s
assert 'Ordinary foreground commands (rtstat, ps, file commands, etc.) are' in s
assert 'rt_explicit_yield_active' in s
# Distinct-slot RT sweep from ZED remains present.
assert 'rt_burst_done_mask' in s and 'rt_pick_ready_sweep' in s
PY
grep -q 'ASSERT CONSOLE_IDLE_TICKS 8' TST/TESTMRT.TST
grep -q 'ASSERT CONSOLE_IDLE_TICKS 8' TST/TESTVWR.TST
grep -q 'START RT10 4' TST/TESTMRT.TST
grep -q 'ASSERT RT_SWEEP_FAIR 4 4' TST/TESTMRT.TST
grep -q 'RUN execmt SONARVWR.EXE' TST/TESTVWR.TST
grep -q 'WAIT LAST' TST/TESTVWR.TST
count=`grep -Ev '^[[:space:]]*(#|$)' TST/ACCEPT.TXT | wc -l | tr -d ' '`
[ "$count" = 45 ]
[ `grep -Ev '^[[:space:]]*(#|$)' TST/ACCEPT.TXT | sort -u | wc -l | tr -d ' '` = 45 ]
while IFS= read -r t; do
  case "$t" in ''|'#'*) continue;; esac
  [ -f "TST/$t" ] || { echo "missing TST/$t"; exit 1; }
  grep -q "build/TST/$t=TST/$t" build.sh || { echo "missing FAT payload $t"; exit 1; }
done < TST/ACCEPT.TXT
! find . -maxdepth 2 -type f -name 'B*.TST' | grep -q .
! find . -maxdepth 2 -type f -name 'TST.LOG' | grep -q .
[ ! -d build ]
echo 'FIX60ZEF CHECK PASS: foreground ownership + bounded MT slack + 45 tests'
