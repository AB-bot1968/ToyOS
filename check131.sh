#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
grep -q 'for(i=0u;i<24u;i++)put("\\n")' "$ROOT/src/vgadrv.c"
grep -q 'put("VGADRV: OK ")' "$ROOT/src/vgadrv.c"
! grep -q 'put("VGADRV: OK\\n")' "$ROOT/src/vgadrv.c"
grep -q 'console_clear();' "$ROOT/src/kernel.c"
echo 'CHECK131 PASS: VGADRV success message is positioned on the bottom text row before the shell prompt'
