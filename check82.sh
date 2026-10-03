#!/bin/sh
# v65.5: EXE1 argument retrieval through a dedicated safe syscall.
set -eu
python3 - <<'PY'
from pathlib import Path
k=Path('src/kernel.c').read_text()
v=Path('src/vgadrv.c').read_text()
assert '#define SYS_EXEC_ARG       34u' in k
assert 'case SYS_EXEC_ARG:' in k
assert 'arg_len=sc(SYS_EXEC_ARG,0u,(uint32_t)file,EXEC_ARG_SIZE);' in v
assert 'pushl %edx' not in v
assert 'pushl %ecx' not in v
assert 'pushl %ebx' not in v
assert 'jmp _program_main' in v
print('PASS: VGADRV obtains FILE through SYS_EXEC_ARG(34), no register ABI dependency')
PY
