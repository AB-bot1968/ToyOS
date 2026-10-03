#!/bin/sh
set -eu
grep -q 'SYS_EVENT_LOG.*62u' src/kernel.c
grep -q 'PROCESS_EVENT_MAX 64u' src/kernel.c
grep -q 'case SYS_EVENT_LOG' src/kernel.c
grep -q 'EVENT_REASON ' src/user_shell.c
grep -q 'TST/TESTEVT.TST' build.sh
grep -q 'RUN rtstat reset' TST/TESTLOAD.TST
grep -q 'RUN rtstat reset' TST/TESTKEY.TST
grep -q 'ASSERT EVENT_REASON 2' TST/TESTEVT.TST
[ ! -d build ]
echo 'FIX41 eventlog static check: OK'
python3 - <<'PY'
import pathlib, sys
lines=pathlib.Path('build.sh').read_text(errors='ignore').splitlines()
line=next((x for x in lines if 'mkfat16.exe build/disk.img' in x), '')
bad=[]
for tok in line.split():
    if '=' not in tok or tok.startswith('$'):
        continue
    src,dst=tok.split('=',1)
    if '$' in dst:
        dst=dst.split('$',1)[0]
    name=dst.rsplit('/',1)[-1]
    if not name:
        continue
    if '.' in name:
        base,ext=name.rsplit('.',1)
    else:
        base,ext=name,''
    if not (1 <= len(base) <= 8 and len(ext) <= 3):
        bad.append(dst)
if bad:
    print('FIX41A FAT 8.3 audit FAILED:', ', '.join(sorted(set(bad))))
    sys.exit(1)
print('FIX41A FAT 8.3 audit: OK')
PY
