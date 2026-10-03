#!/bin/sh
set -eu
python3 - <<'PY'
p='src/vgadrv.c'
s=open(p,encoding='utf-8').read()
assert 'for(i=0u;i<23u;i++)put("\\n");' in s
assert 'put("VGADRV: OK \\n");' in s
assert 'for(i=0u;i<24u;i++)put("\\n");' not in s
assert 'put("VGADRV: OK ");' not in s
print('CHECK132 PASS: VGADRV message is on row 23 and shell prompt starts on row 24')
PY
