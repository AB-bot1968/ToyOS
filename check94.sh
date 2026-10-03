#!/bin/sh
set -eu
grep -q 'vga_y=H-2u;' src/kernel.c
grep -q 'vga_x=0;' src/kernel.c
echo 'check94: PASS'
