#!/bin/sh
# check64.sh — v60.4: регрессионная проверка остановки после MT25.EXE.
# Проверяет именно тот дефект, который проявлялся на Windows/W64DevKit:
# путь HOSTFILE без '=' должен превращаться только в имя файла FAT16,
# а не в каталог build/... внутри образа.
set -eu
cd "$(dirname "$0")"
HOST="-O2 -std=c99 -Wall -Wextra -Werror"
TMP="build/check64"
rm -rf "$TMP"
mkdir -p "$TMP/host"

printf '%s\n' '[1/3] mkfat16 source audit'
grep -F 'static const char *host_basename' tools/mkfat16.c >/dev/null
grep -F 'const char *base=host_basename(spec)' tools/mkfat16.c >/dev/null
grep -F 'mkfat16 finished without creating build/disk.img' build.sh >/dev/null

printf '%s\n' '[2/3] three compilation passes'
gcc $HOST -fsyntax-only tools/mkfat16.c
gcc $HOST -c tools/mkfat16.c -o "$TMP/mkfat16.o"
gcc $HOST -fno-omit-frame-pointer -c tools/mkfat16.c -o "$TMP/mkfat16_fp.o"
test -s "$TMP/mkfat16.o"
test -s "$TMP/mkfat16_fp.o"
gcc $HOST tools/mkfat16.c -o "$TMP/mkfat16.exe"
gcc $HOST tools/fat16check.c -o "$TMP/fat16check.exe"
gcc $HOST tools/mklayout.c -o "$TMP/mklayout.exe"
as --32 src/layout_loader.S -o "$TMP/layout_loader.o"
ld -m i386pe --image-base 0 -e layout_loader_entry -T layout_loader.ld -o "$TMP/layout_loader.pe" "$TMP/layout_loader.o"
objcopy --only-section=.text -O binary "$TMP/layout_loader.pe" "$TMP/layout_loader.bin"

printf '%s\n' '[3/3] image regression'
for n in BOOT.BIN HELLO.EXE COMDRV.EXE LOADER.EXE NETDRV.EXE NET.CFG QPASS.EXE QPORT.EXE QFROMEXE.EXE QSTOP.EXE QNESTED.EXE QFILE.EXE QSIZE.EXE; do printf x > "$TMP/host/$n"; done
for i in `seq 1 25`; do printf x > "$TMP/host/MT`printf '%02d' "$i"`.EXE"; done
printf x > "$TMP/host/NET.CFG"
"$TMP/mkfat16.exe" "$TMP/disk.img" --part-lba 768 --free-percent 25 \
  "$TMP/host/BOOT.BIN" "$TMP/host/HELLO.EXE" "$TMP/host/COMDRV.EXE" "$TMP/host/LOADER.EXE" "$TMP/host/NETDRV.EXE" "$TMP/host/NET.CFG" \
  "$TMP/host/QPASS.EXE" "$TMP/host/QPORT.EXE" "$TMP/host/QFROMEXE.EXE" "$TMP/host/QSTOP.EXE" "$TMP/host/QNESTED.EXE" "$TMP/host/QFILE.EXE" "$TMP/host/QSIZE.EXE" \
  "$TMP/host/MT01.EXE" "$TMP/host/MT02.EXE" "$TMP/host/MT03.EXE" "$TMP/host/MT04.EXE" "$TMP/host/MT05.EXE" "$TMP/host/MT06.EXE" "$TMP/host/MT07.EXE" "$TMP/host/MT08.EXE" "$TMP/host/MT09.EXE" "$TMP/host/MT10.EXE" "$TMP/host/MT11.EXE" "$TMP/host/MT12.EXE" "$TMP/host/MT13.EXE" "$TMP/host/MT14.EXE" "$TMP/host/MT15.EXE" "$TMP/host/MT16.EXE" "$TMP/host/MT17.EXE" "$TMP/host/MT18.EXE" "$TMP/host/MT19.EXE" "$TMP/host/MT20.EXE" "$TMP/host/MT21.EXE" "$TMP/host/MT22.EXE" "$TMP/host/MT23.EXE" "$TMP/host/MT24.EXE" "$TMP/host/MT25.EXE" \
  "$TMP/host/HELLO.EXE=BIN/HELLO.EXE" "$TMP/host/NETDRV.EXE=BIN/NETDRV.EXE" "$TMP/host/NET.CFG=DOC/NET.CFG"
test -s "$TMP/disk.img"
imgbytes=`wc -c < "$TMP/disk.img" | tr -d ' '`
imgsecs=$((imgbytes/512)); fatsecs=$((imgsecs-768))
# Minimal valid descriptor for this historical filesystem regression.
"$TMP/mklayout.exe" "$TMP/layout_loader.bin" "$TMP/layout.bin" 2 1 1 64 701 768 "$fatsecs" "$imgsecs" 256 25 32256 655360
dd if="$TMP/layout.bin" of="$TMP/disk.img" bs=512 seek=1 count=1 conv=notrunc 2>/dev/null
"$TMP/fat16check.exe" "$TMP/disk.img" MT25.EXE >/dev/null
"$TMP/fat16check.exe" "$TMP/disk.img" BIN/HELLO.EXE >/dev/null
"$TMP/fat16check.exe" "$TMP/disk.img" DOC/NET.CFG >/dev/null
printf '%s\n' 'PASS: v60.4 mkfat16 basename and post-MT25 image creation regression'
