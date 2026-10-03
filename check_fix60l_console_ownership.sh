#!/bin/sh
set -eu
K=src/kernel.c; U=src/user_shell.c
fail(){ echo "FIX60L/60N console ownership: FAIL: $1"; exit 1; }
grep -q 'if(sched_active&&!exe_active){f->eax=2u;break;}' "$K" || fail console_return
grep -q 'FIX60N: while detached MT work exists' "$U" || fail ring3_window
python3 - <<'PY'
from pathlib import Path
s=Path('src/kernel.c').read_text(); a=s.index('case SYS_CONSOLE_READ:'); b=s.index('case SYS_CONSOLE_POLL:',a)
assert 'int mid=sched_pick_next' not in s[a:b]
PY
test -f TST/TESTMTL.TST || fail TESTMTL
test -f TST/TESTSONL.TST || fail TESTSONL
echo 'FIX60L/60N console ownership: PASS'
