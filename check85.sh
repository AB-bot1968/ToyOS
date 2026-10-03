#!/bin/sh
set -eu
# SPLASH.RAW v65.8 содержит ровно 320x200 однобайтных индексов палитры.
test -f resources/SPLASH.RAW
test "`wc -c < resources/SPLASH.RAW | tr -d '[:space:]'`" -eq 64000
# Встроенная заставка должна содержать исходный логотип/инструкцию в preview.
test -f resources/SPLASH_PREVIEW.png
# Контролируем наличие нового syscall в ядре.
grep -q '#define SYS_VIDEO_TEXT[[:space:]]*35u' src/kernel.c
grep -q 'case SYS_VIDEO_TEXT:' src/kernel.c
echo 'check85: PASS'
