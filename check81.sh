#!/bin/sh
# v65.5: VGADRV no longer relies on startup-register arguments.
set -eu
python3 - <<'PY'
from pathlib import Path
s = Path('src/vgadrv.c').read_text()
expected = 'void program_main(void)'
if expected not in s:
    raise SystemExit('VGADRV no-argument C entry is missing')
if 'arg_len=sc(SYS_EXEC_ARG,0u,(uint32_t)file,EXEC_ARG_SIZE);' not in s:
    raise SystemExit('VGADRV SYS_EXEC_ARG call is missing')
if '__asm__ volatile("" : "=d"(file));' in s:
    raise SystemExit('old undefined EDX input inline-asm remains')
for x in ['pushl %edx','pushl %ecx','pushl %ebx']:
    if x in s:
        raise SystemExit('obsolete register argument bridge remains: '+x)
if 'jmp _program_main' not in s:
    raise SystemExit('EXE1 entry jump is missing')
print('PASS: VGADRV uses no-argument entry and retrieves FILE via SYS_EXEC_ARG(34)')
PY
