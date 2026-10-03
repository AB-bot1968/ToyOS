#!/bin/sh
set -eu
# v65.8: проверяем нативный ресурс 320x200 и полный VGA mode-set обратно в 80x25.
test -f resources/SPLASH.RAW
test "`wc -c < resources/SPLASH.RAW | tr -d '\n[:space:]'`" -eq 64000
grep -q "#define IMAGE_WIDTH[[:space:]]*320u" src/vgadrv.c
grep -q "#define IMAGE_HEIGHT[[:space:]]*200u" src/vgadrv.c
grep -q "outb(VGA_SEQ,0x01u);" src/kernel.c
grep -q "v|0x20u" src/kernel.c
grep -q "outb(VGA_MISC_WRITE,0x67u);" src/kernel.c
grep -q "outb(VGA_SEQ_DATA,0x01u);" src/kernel.c
grep -q "outb(VGA_SEQ_DATA,0x03u);" src/kernel.c
grep -q "outb(0x3c6u,0xffu);" src/kernel.c
grep -q "uint8_t attr\[21\]" src/kernel.c
echo 'check88: PASS'
