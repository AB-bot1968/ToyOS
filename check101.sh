#!/bin/sh
set -eu
printf '[check101] PNG2RAW self-test conversion... '
test -s build/png2raw.exe
test -s resources/SPLASH_PREVIEW.png
rm -f build/check101.raw
./build/png2raw.exe resources/SPLASH_PREVIEW.png build/check101.raw >/dev/null
test -f build/check101.raw
test "$(wc -c < build/check101.raw | tr -d ' ')" -eq 64000
test -s build/check101.raw
echo PASS
