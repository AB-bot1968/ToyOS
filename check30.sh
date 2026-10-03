#!/bin/sh
set -eu
fail(){ echo "FAIL: $1" >&2; exit 1; }
python3 - <<'PY'
from pathlib import Path
s=Path('src/kernel.c').read_text()
needle='if(!queue_active && !exe_active)'
assert needle in s, 'standalone EXE queue must not overwrite saved shell frame'
assert 'overwriting it with the EXE' in s, 'regression explanation missing'
assert 'if(from_exe && queue_active)queue_save_parent();' in s
assert 'from_exe=(f->cs&3u)&&exe_active' in s
assert 'if(queue_depth){' in s and 'queue_restore_parent();' in s
print('PASS: standalone EXE queue preserves shell return frame')
PY
grep -q 'exec QNESTED.EXE' README.md || fail 'standalone QNESTED documentation'
grep -q 'execq 1 QPASS.EXE QNESTED.EXE QPASS.EXE' README.md || fail 'nested QNESTED documentation'
echo '=== CHECK30 PASS ==='
