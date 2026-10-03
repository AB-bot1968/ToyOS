#!/bin/sh
set -eu
# Проверяем, что единственная точка возврата из VGADRV — kernel syscall 35,
# а старого Ring-3 набора регистрового восстановления текстового режима нет.
grep -q '#define SYS_VIDEO_TEXT[[:space:]]*35u' src/vgadrv.c
grep -q 'SYS_VIDEO_TEXT,0,0,0' src/vgadrv.c
if grep -q 'static int set_text_mode' src/vgadrv.c; then
  echo 'check84: FAIL: legacy Ring-3 text-mode restore remains'
  exit 1
fi
echo 'check84: PASS'
