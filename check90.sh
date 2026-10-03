#!/bin/sh
set -eu
# v65.9: нативный graphics resource и отсутствие старого 2x2 scaler.
test "`wc -c < resources/SPLASH.RAW | tr -d '[:space:]'`" -eq 64000
grep -q '#define IMAGE_WIDTH      320u' src/vgadrv.c
grep -q '#define IMAGE_HEIGHT     200u' src/vgadrv.c
if grep -q 'x\*2u' src/vgadrv.c; then
  echo 'check90: FAIL: old 2x2 image scaler remains' >&2
  exit 1
fi
grep -q 'v\[offset+i\]=b\[i\]' src/vgadrv.c
echo 'check90: PASS'
