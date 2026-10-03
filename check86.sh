#!/bin/sh
set -eu
# Возврат VGA должен выполняться kernel-side, а не локальной таблицей режима в VGADRV.
grep -q '#define SYS_VIDEO_TEXT[[:space:]]*35u' src/kernel.c
grep -q 'case SYS_VIDEO_TEXT:' src/kernel.c
grep -q 'vga_restore_text_mode_hw' src/kernel.c
if grep -q 'static int set_text_mode' src/vgadrv.c; then
  echo 'check86: FAIL: VGADRV still contains local text restore'
  exit 1
fi
grep -q 'SYS_VIDEO_TEXT,0,0,0' src/vgadrv.c
echo 'check86: PASS'
