#!/bin/sh
set -eu
ROOT=`CDPATH= cd -- "$(dirname -- "$0")" && pwd`
cd "$ROOT"
python3 - <<'PY2'
from pathlib import Path
import hashlib
s=Path('src/kernel.c').read_text()
regions=[
 ('rt_switch_to_shell','static void rt_switch_to_shell(struct frame*f){','static void rt_wait(struct frame*f){','ca2e38f76c15267f51945570763dc73433482c45c13bc52b4ed7fd305bd73bc6'),
 ('rt_wait','static void rt_wait(struct frame*f){','/* FIX35A: terminate a known RT slot','1d3f5a7d6cbe04d6f16a50e1c5b6c8cadaac780b222d6220a229aeb38fff018d'),
 ('sched_irq_tick','static void sched_irq_tick(struct frame*f){','static void sched_block(struct frame*f){','ba3ea226372e88b902d038c82cf527634c6fd90c944a4f9762e87295069c5611'),
]
for name,a,b,want in regions:
    i=s.index(a); j=s.index(b,i+1)
    got=hashlib.sha256(s[i:j].encode()).hexdigest()
    assert got==want,(name,got,want)
# Prompt redraw is bookkeeping only; console-read must still have no context switch.
a=s.index('case SYS_CONSOLE_READ:'); b=s.index('case SYS_CONSOLE_POLL:',a); blk=s[a:b]
assert 'rt_switch_to(' not in blk and 'load_cr3(' not in blk
assert 'console_redraw_pending' in blk and 'f->eax=3u' in blk
assert 'console_background_write_note' in s and 'console_background_done' in s
PY2
grep -q 'ASSERT CONSOLE_IDLE_TICKS 12' TST/TESTMRT.TST
grep -q 'ASSERT CONSOLE_IDLE_TICKS 64' TST/TESTMRT.TST
grep -q 'ASSERT MT_PROGRESS 1 3' TST/TESTMRT.TST
grep -q 'ASSERT MT_PROGRESS 2 3' TST/TESTMRT.TST
grep -q 'ASSERT MT_HEARTBEAT_WAIT 2 12 800' TST/TESTMRT.TST
grep -q 'ASSERT MT_HEARTBEAT_WAIT 2 24 1200' TST/TESTMRT.TST
[ `grep -c '^ASSERT CONSOLE_REDRAW$' TST/TESTVWR.TST` = 4 ]
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
echo 'FIX60ZEG CHECK PASS: FIX60ZEF scheduler preserved + idle-shell TESTMRT contract + prompt redraw'
