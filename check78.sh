#!/bin/sh
# v65: внешний графический ресурс и Ring-3 VGA driver wiring.
set -eu
fail(){ echo "FAIL: $1" >&2; exit 1; }
pass(){ echo "PASS: $1"; }
python3 - <<'PY'
from pathlib import Path
k=Path('src/kernel.c').read_text()
sh=Path('src/user_shell.c').read_text()
v=Path('src/vgadrv.c').read_text()
b=Path('build.sh').read_text()
r=Path('resources/SPLASH.RAW')
if not r.exists() or r.stat().st_size != 64000:
    raise SystemExit('SPLASH.RAW must be exactly 64000 bytes')
for x in ['#define SYS_VIDEO_MAP      32u','#define SYS_VIDEO_UNMAP    33u','case SYS_VIDEO_MAP:','case SYS_VIDEO_UNMAP:','0x000a0000u']:
    if x not in k: raise SystemExit('kernel video support missing: '+x)
for x in ['exec VGADRV.EXE FILE','exec_vga_command','sys_exec_args(name,args)']:
    if x not in sh: raise SystemExit('shell video command missing: '+x)
for x in ['SYS_FILE_OPEN','SYS_FILE_READ','SYS_PORT_OUT8','SYS_VIDEO_MAP','SYS_VIDEO_UNMAP','0x000a0000u','set_mode13','IMAGE_WIDTH','IMAGE_HEIGHT']:
    if x not in v: raise SystemExit('VGADRV missing: '+x)
for x in ['src/vgadrv.c','build/VGADRV.EXE=VGADRV.EXE','build/SPLASH.RAW=SPLASH.RAW']:
    if x not in b: raise SystemExit('build wiring missing: '+x)
PY
pass "external SPLASH.RAW and Ring-3 VGADRV wiring"
