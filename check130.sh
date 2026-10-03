#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
grep -q '^exec VGADRV.EXE /SPLASH.RAW$' "$ROOT/resources/AUTOSTART.SH"
grep -q 'image_path\[0\].*=.*\/' "$ROOT/src/vgadrv.c"
grep -q 'if(fd==SYS_FAIL)fd=file_open(file_norm);' "$ROOT/src/vgadrv.c"
grep -q 'sc(SYS_VIDEO_TEXT,0,0,0);' "$ROOT/src/vgadrv.c"
grep -q 'vga_y=0;' "$ROOT/src/kernel.c"
test "$(wc -c < "$ROOT/resources/SPLASH.RAW" | tr -d '[:space:]')" -eq 64000
echo 'CHECK130 PASS: splash path, VGA failure recovery and clean text-console start verified'
