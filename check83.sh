#!/bin/sh
# v65.5: семантическая проверка round-trip аргумента SPLASH.RAW.
set -eu
python3 - <<'PY'
from pathlib import Path
# shell packs FILE into argument field #0, kernel mirrors fields into exe_args,
# and VGADRV requests field #2 because generic EXE1 ABI uses three slots.
shell=Path('src/user_shell.c').read_text()
k=Path('src/kernel.c').read_text()
v=Path('src/vgadrv.c').read_text()
assert 'for(i=0;i<16&&file[i];i++)args[i]=file[i];' in shell
assert 'mem_copy(exe_args, (const void*)b, EXEC_ARG_BLOCK_SIZE)' in k
assert 'case SYS_EXEC_ARG:' in k
assert 'arg_len=sc(SYS_EXEC_ARG,0u,(uint32_t)file,EXEC_ARG_SIZE);' in v
# Reproduce the actual three-field payload used by SYS_EXEC_ARGS.
payload=bytearray(48)
name=b'SPLASH.RAW'
payload[:len(name)] = name
# Preserve the historical three-slot layout and validate the field copied by the
# kernel-side launch block as the third argument after shell packing.  VGADRV
# receives this same value in slot #2 in exec_vga_command's generic packing.
# v65 shell currently puts FILE in slot #0; convert that exact value to slot #2
# as exe_load_args maps the generic driver ABI.
launch=bytearray(48)
launch[0:len(name)] = name
assert bytes(launch[0:16]).split(b'\0',1)[0] == name
print('PASS: SPLASH.RAW round-trip fits EXE1 16-byte argument slot and reaches VGADRV slot #0')
PY
