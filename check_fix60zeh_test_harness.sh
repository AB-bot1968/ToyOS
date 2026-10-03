#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
cd "$ROOT"

KERNEL_SHA=$(sha256sum src/kernel.c | awk '{print $1}')
[ "$KERNEL_SHA" = "3af4e5b2a9c8610eaee3efb1b533836dadf843e1b212922c9fee354c860dd001" ] || { echo "FAIL: kernel.c differs from stable FIX60ZEG"; exit 1; }

grep -q 'if(r==3u)continue;if(r!=2u)return 0u' src/user_shell.c || { echo "FAIL: redraw-aware CONSOLE_IDLE_TICKS missing"; exit 1; }
grep -q 'TEST SOURCE: bytes=' src/user_shell.c || { echo "FAIL: runtime TST fingerprint missing"; exit 1; }
grep -q 'TEST HARNESS FAIL line ' src/user_shell.c || { echo "FAIL: RUN rejection diagnostic missing"; exit 1; }
grep -q 'staged test differs from source' build.sh || { echo "FAIL: source->build TST cmp guard missing"; exit 1; }

[ "$(sed -n '48p' TST/TESTMRT.TST)" = 'RUN rtstat' ] || { echo "FAIL: TESTMRT line 48 changed"; exit 1; }
[ "$(sed -n '16p' TST/TESTMRT.TST)" = 'ASSERT CONSOLE_IDLE_TICKS 12' ] || { echo "FAIL: TESTMRT line 16 changed"; exit 1; }
[ "$(sed -n '17p' TST/TESTMRT.TST)" = 'ASSERT MT_PROGRESS 1 3' ] || { echo "FAIL: TESTMRT line 17 changed"; exit 1; }
[ "$(sed -n '18p' TST/TESTMRT.TST)" = 'ASSERT MT_PROGRESS 2 3' ] || { echo "FAIL: TESTMRT line 18 changed"; exit 1; }

grep -qx 'TESTMRT.TST' TST/ACCEPT.TXT || { echo "FAIL: TESTMRT not in ACCEPT"; exit 1; }
COUNT=$(grep -Ev '^[[:space:]]*(#|$)' TST/ACCEPT.TXT | sort -u | wc -l | tr -d ' ')
[ "$COUNT" = 45 ] || { echo "FAIL: expected 45 unique acceptance tests, got $COUNT"; exit 1; }

python - <<'PY'
from pathlib import Path
p=Path('TST/TESTMRT.TST')
b=p.read_bytes(); h=2166136261
for x in b:
    h ^= x
    h = (h*16777619) & 0xffffffff
assert len(b)==1685, (len(b), 'unexpected TESTMRT bytes')
assert h==3848550741, (h, 'unexpected TESTMRT FNV')
print('TESTMRT source fingerprint PASS: bytes=1685 fnv1a32=3848550741')
PY

if find . -type f -name 'TST.LOG' -print -quit | grep -q .; then echo 'FAIL: TST.LOG present'; exit 1; fi
if find TST -maxdepth 1 -type f -name 'B*.TST' -print -quit | grep -q .; then echo 'FAIL: B*.TST present'; exit 1; fi
if [ -d build ]; then echo 'FAIL: build/ present'; exit 1; fi

echo 'FIX60ZEH CHECK PASS: scheduler preserved; redraw-aware idle test; source fingerprint guard active'
