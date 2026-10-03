#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
cd "$ROOT"

EXPECTED_KERNEL_SHA=3af4e5b2a9c8610eaee3efb1b533836dadf843e1b212922c9fee354c860dd001
ACTUAL_KERNEL_SHA=$(sha256sum src/kernel.c | awk '{print $1}')
[ "$ACTUAL_KERNEL_SHA" = "$EXPECTED_KERNEL_SHA" ] || { echo "FAIL: kernel.c changed" >&2; exit 1; }

grep -q 'MT_HEARTBEAT_IDLE_WAIT ' src/user_shell.c
grep -q 'r=sys_console_read(&c,1u)' src/user_shell.c
grep -q 'if(r==3u)continue;if(r!=2u)return 0u' src/user_shell.c

test "$(sed -n '43p' TST/TESTMRT.TST)" = 'ASSERT MT_HEARTBEAT_IDLE_WAIT 2 12 800'
test "$(sed -n '46p' TST/TESTMRT.TST)" = 'ASSERT MT_HEARTBEAT_IDLE_WAIT 2 24 1200'
test "$(wc -l < TST/TESTMRT.TST | tr -d ' ')" = '58'

grep -q 'ASSERT CONSOLE_IDLE_TICKS 64' TST/TESTMRT.TST
grep -q 'ASSERT RT_SWEEP_FAIR 4 4' TST/TESTMRT.TST
grep -q 'ASSERT SONAR_LOG_MIN 8' TST/TESTMRT.TST

grep -q 'FIX60ZEI: heartbeat-проверка обязана соблюдать idle-shell контракт' TOYOS_ARCHITECTURE_VISION.md
grep -q 'staged test differs from source' build.sh

python3 - <<'PY'
from pathlib import Path
root=Path('.')
b=(root/'TST/TESTMRT.TST').read_bytes()
h=2166136261
for x in b:
    h ^= x
    h = (h * 16777619) & 0xffffffff
assert len(b)==1692, (len(b),'TESTMRT bytes')
assert h==2456257148, (h,'TESTMRT FNV')

names=[]
for raw in (root/'TST/ACCEPT.TXT').read_text().splitlines():
    s=raw.strip()
    if not s or s.startswith('#'): continue
    names.append(s)
assert len(names)==45, len(names)
assert len(set(names))==45, 'duplicate acceptance entry'
for n in names:
    assert (root/'TST'/n).is_file(), n

build=(root/'build.sh').read_text()
for n in names:
    token=f'build/TST/{n}=TST/{n}'
    assert token in build, token

assert not (root/'build').exists(), 'build/ must not be packaged'
assert not list(root.rglob('TST.LOG')), 'TST.LOG forbidden'
assert not list((root/'TST').glob('B*.TST')), 'B*.TST forbidden'
print('FIX60ZEI CHECK PASS: stable kernel + idle-aware heartbeat + 45 acceptance tests')
PY
