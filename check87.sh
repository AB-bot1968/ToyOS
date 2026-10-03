#!/bin/sh
set -eu
# v65.8: контроль стандартного цветного VGA text mode 03h (80x25).
grep -q 'static const uint8_t seq\[5\]={0x00,0x00,0x03,0x00,0x03}' src/kernel.c
grep -q '0x5f,0x4f,0x50,0x82,0x55,0x81,0xbf,0x1f' src/kernel.c
grep -q 'static const uint8_t gc\[9\]={0x00,0x00,0x00,0x00,0x00,0x10,0x0e,0x0f,0xff}' src/kernel.c
grep -q 'outb(VGA_MISC_WRITE,0x67u)' src/kernel.c
grep -q 'outb(VGA_ATTR,0x20u)' src/kernel.c
# Новый syscall очищает текстовый буфер после аппаратного mode-set.
grep -A12 'case SYS_VIDEO_TEXT:' src/kernel.c | grep -q 'console_clear();'
echo 'check87: PASS'
