#!/bin/sh
# v65.5: VGADRV получает аргумент запуска через SYS_EXEC_ARG(34).
set -eu
python3 - <<'PY'
from pathlib import Path
k = Path('src/kernel.c').read_text()
v = Path('src/vgadrv.c').read_text()
if '#define SYS_EXEC_ARG       34u' not in k:
    raise SystemExit('kernel SYS_EXEC_ARG definition is missing')
if 'case SYS_EXEC_ARG:' not in k:
    raise SystemExit('kernel SYS_EXEC_ARG handler is missing')
if 'arg_len=sc(SYS_EXEC_ARG,0u,(uint32_t)file,EXEC_ARG_SIZE);' not in v:
    raise SystemExit('VGADRV does not request FILE through SYS_EXEC_ARG(34)')
if 'void program_main(void)' not in v:
    raise SystemExit('VGADRV no-argument program_main entry is missing')
if 'pushl %edx' in v or 'pushl %ecx' in v or 'pushl %ebx' in v:
    raise SystemExit('obsolete register argument bridge remains in VGADRV')
if 'jmp _program_main' not in v:
    raise SystemExit('VGADRV EXE1 entry jump is missing')
print('PASS: VGADRV uses dedicated SYS_EXEC_ARG(34); no register argument ABI dependency')
PY
