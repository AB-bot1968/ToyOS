#!/bin/sh
set -eu
# v65.9: text mode uses standard VGA mode-03 values and the mode-13 table is complete.
grep -q 'static const uint8_t seq\[5\]={0x00,0x00,0x03,0x00,0x03};' src/kernel.c
grep -q '0x5f,0x4f,0x50,0x82,0x55,0x81,0xbf,0x1f' src/kernel.c
grep -q 'static const uint8_t gc\[9\]={0x00,0x00,0x00,0x00,0x00,0x10,0x0e,0x0f,0xff};' src/kernel.c
grep -q 'uint8_t attr\[21]' src/kernel.c
grep -q 'static const uint8_t a\[21]' src/vgadrv.c
echo 'check92: PASS'
