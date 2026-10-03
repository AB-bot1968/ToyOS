#!/bin/sh
set -eu
fail(){ echo "CHECK FAILED: $1"; exit 1; }
B=build
[ -f "$B/boot.bin" ] || B=build_audit
[ -f "$B/boot.bin" ] || fail boot.bin
[ -f "$B/kernel.bin" ] || fail kernel.bin
[ -f "$B/disk.img" ] || fail disk.img
[ -f "$B/layout.bin" ] || fail layout.bin
[ -f "$B/isr.o" ] || fail isr.o
[ -f "$B/user_shell.o" ] || fail user_shell.o
[ `wc -c < "$B/boot.bin"` -eq 512 ] || fail "boot size"
[ `wc -c < "$B/layout.bin"` -eq 512 ] || fail "layout size"
[ "`od -An -t x1 -j 510 -N 2 "$B/boot.bin" | tr -d ' \n'`" = 55aa ] || fail "boot signature"
[ "`od -An -t x1 -j 0 -N 1 "$B/kernel.bin" | tr -d ' \n'`" = ea ] || fail "kernel far jump opcode"
[ -f liker.ld ] && [ -f layout_loader.ld ] && [ -f src/boot.S ] && [ -f src/layout_loader.S ] && [ -f src/kernel.c ] && [ -f src/user_shell.c ] || fail "required sources"
[ -z "`nm -u "$B/kernel.pe"`" ] || fail "unresolved symbols"
[ "`od -An -t x1 -j 510 -N 2 "$B/disk.img" | tr -d ' \n'`" = 55aa ] || fail "disk boot signature"
./build/layoutcheck.exe "$B/disk.img" >/dev/null || fail "dynamic layout"
# Kernel starts at descriptor field kernel_lba; compare first byte.
KLBA=`od -An -t u4 -j $((512+16)) -N 4 "$B/disk.img" | tr -d ' '`
[ -n "$KLBA" ] || fail kernel-lba
[ "`dd if="$B/disk.img" bs=512 skip="$KLBA" count=1 2>/dev/null | od -An -t x1 -N 1 | tr -d ' \n'`" = "`od -An -t x1 -N 1 "$B/kernel.bin" | tr -d ' \n'`" ] || fail "kernel overlay"
echo "10/10 checks passed (FIX53 dynamic layout)"
