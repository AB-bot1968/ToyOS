#!/bin/sh
set -eu
fail(){ echo "CHECK45 FAIL: $*"; exit 1; }
python3 - <<'PY'
from pathlib import Path
s=Path('src/execmt_task.c').read_text()
assert 'char line[8];' in s
assert "line[0]='T';line[1]='a';line[2]='s';line[3]='k';line[4]=' ';" in s
assert "line[7]='\\n';" in s
assert 'put(line);' in s
assert 'put("Task ");' not in s
assert 'dec(' not in s
k=Path('src/kernel.c').read_text()
assert 'static void sched_announce' in k
assert 'sched_announce(&sched_tasks[0]);' in k
print('PASS: EXECMT Task NN output is assembled and emitted as one complete line; scheduler diagnostic remains')
PY
echo 'CHECK45 PASS'
