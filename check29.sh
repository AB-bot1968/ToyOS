#!/bin/sh
set -eu
fail(){ echo "FAIL: $1" >&2; exit 1; }
grep -q '#define QUEUE_MAX_DEPTH 4u' src/kernel.c || fail 'nested queue depth constant'
grep -q 'struct queue_state' src/kernel.c || fail 'nested queue state'
grep -q 'queue_save_parent();' src/kernel.c || fail 'parent queue save'
grep -q 'queue_restore_parent();' src/kernel.c || fail 'parent queue restore'
grep -q 'from_exe=(f->cs&3u)&&exe_active' src/kernel.c || fail 'EXE queue detection'
grep -q 'src/queue_tests/qnested.c' build.sh || fail 'QNESTED build entry'
grep -q 'src/queue_tests/qfile.c' build.sh || fail 'QFILE build entry'
grep -q 'build/QNESTED.EXE' build.sh || fail 'QNESTED FAT installation'
grep -q 'build/QFILE.EXE' build.sh || fail 'QFILE FAT installation'
grep -q 'SYS_FILE_OPEN' src/queue_tests/qfile.c || fail 'QFILE file open'
grep -q 'MODE_APPEND' src/queue_tests/qfile.c || fail 'QFILE append mode'
grep -q 'execq 1 QPASS.EXE QNESTED.EXE QPASS.EXE' README.md || fail 'nested queue example'
grep -q 'cat EXELOG.TXT' README.md || fail 'file append example'
python3 - <<'PY'
from pathlib import Path
s=Path('src/kernel.c').read_text()
assert 'queue_save_parent();' in s and 'queue_restore_parent();' in s
assert 'queue_depth>=QUEUE_MAX_DEPTH' in s
print('PASS: nested queue state stack, EXE-origin detection, and EXE1 file append tests are present')
PY
echo '=== CHECK29 PASS ==='
# Standalone EXE1 may also create a top-level queue; only an EXE inside an
# already active queue pushes a parent queue state.
grep -q 'from_exe=(f->cs&3u)&&exe_active' src/kernel.c || { echo 'check29: missing EXE queue detection'; exit 1; }
grep -q 'if(from_exe && queue_active)queue_save_parent' src/kernel.c || { echo 'check29: missing conditional parent save'; exit 1; }
grep -q 'if(!queue_active)' src/kernel.c || { echo 'check29: missing shell-frame save'; exit 1; }
echo 'check29: nested/top-level EXE queue transition checks passed'
