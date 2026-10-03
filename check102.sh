#!/bin/sh
set -eu
printf '[check102] PNG2RAW conversion smoke test... '
mkdir -p build
test -s resources/SPLASH_PREVIEW.png
test -s tools/PNG2RAW.EXE
./tools/PNG2RAW.EXE resources/SPLASH_PREVIEW.png build/check102_splash.raw >/dev/null
size=`wc -c < build/check102_splash.raw | tr -d '[:space:]'`
test "$size" -eq 64000
# Повторный запуск должен быть детерминированным.
./tools/PNG2RAW.EXE resources/SPLASH_PREVIEW.png build/check102_splash2.raw >/dev/null
cmp build/check102_splash.raw build/check102_splash2.raw >/dev/null
echo PASS
