#!/bin/sh
set -eu
fail(){ echo "FIX53-LAYOUT CHECK FAILED: $1" >&2; exit 1; }
[ -f src/layout_loader.S ] || fail layout-loader-source
[ -f tools/mklayout.c ] || fail mklayout-source
[ -f tools/layoutcheck.c ] || fail layoutcheck-source
[ -f TST/TESTLAY.TST ] || fail TESTLAY
[ -x check_fix53_layout_variants.sh ] || fail layout-variants-checker
[ ! -f src/comdrv_.c ] || fail COMDRV_-source-still-present
[ ! -f TST/TESTCOM.TST ] || fail TESTCOM-still-present
[ ! -f COMDRV.CFG ] || fail COMDRV-CFG-still-present
! grep -q '^#define FAT_LBA 512u' src/kernel.c || fail fixed-kernel-fat-lba
! grep -q '^#define FAT_LBA 512u' tools/fat16check.c || fail fixed-checker-fat-lba
! grep -q 'g_part_lba=512u' tools/mkfat16.c || fail fixed-builder-fat-lba
grep -q 'SYS_LAYOUT_INFO.*69u' src/kernel.c || fail layout-syscall
grep -q 'layout_validate' src/kernel.c || fail kernel-layout-validation
grep -q 'fat_mount(layout_word(9))' src/kernel.c || fail dynamic-kernel-fat-mount
grep -q 'KERNEL_GROWTH_PERCENT' build.sh || fail growth-config
grep -q 'KERNEL_RESERVE_MIN' build.sh || fail reserve-config
grep -q 'LAYOUT_GAP_SECTORS' build.sh || fail gap-config
grep -q 'FAT_ALIGN_SECTORS' build.sh || fail alignment-config
grep -q 'FAT_FREE_PERCENT' build.sh || fail free-space-config
grep -q 'KERNEL_LOW_MEM_CEILING=655360' build.sh || fail low-memory-ceiling
grep -q 'check_fix53_layout_variants.sh' build.sh || fail variants-not-in-build
grep -q 'build/layout.bin' build.sh || fail layout-sector-build
grep -q 'seek=1 count=1' build.sh || fail descriptor-lba1-overlay
grep -q 'ASSERT LAYOUT_VALID' TST/TESTLAY.TST || fail layout-runtime-assert
grep -q 'ASSERT LAYOUT_FAT_ALIGNED_SELF' TST/TESTLAY.TST || fail descriptor-alignment-runtime-assert
grep -q 'ASSERT LAYOUT_RESERVE_GAP_SEPARATE' TST/TESTLAY.TST || fail descriptor-gap-runtime-assert
grep -q 'ASSERT LAYOUT_MEMORY_VALID' TST/TESTLAY.TST || fail descriptor-memory-runtime-assert
grep -q 'ASSERT FS_RW' TST/TESTLAY.TST || fail filesystem-runtime-assert
grep -q 'FIX53 — динамическая разметка диска' TOYOS_ARCHITECTURE_VISION.md || fail architecture-vision
# Host tools must compile independently.
gcc -O2 -std=c99 -Wall -Wextra -Werror tools/mkfat16.c -o /tmp/toyos_fix53_mkfat16
gcc -O2 -std=c99 -Wall -Wextra -Werror tools/fat16check.c -o /tmp/toyos_fix53_fat16check
gcc -O2 -std=c99 -Wall -Wextra -Werror tools/mklayout.c -o /tmp/toyos_fix53_mklayout
gcc -O2 -std=c99 -Wall -Wextra -Werror tools/layoutcheck.c -o /tmp/toyos_fix53_layoutcheck
as --32 src/boot.S -o /tmp/toyos_fix53_boot.o
as --32 src/layout_loader.S -o /tmp/toyos_fix53_layout_loader.o
ld -m i386pe --image-base 0 -e layout_loader_entry -T layout_loader.ld -o /tmp/toyos_fix53_layout_loader.pe /tmp/toyos_fix53_layout_loader.o
objcopy --only-section=.text -O binary /tmp/toyos_fix53_layout_loader.pe /tmp/toyos_fix53_layout_loader.bin
test `wc -c < /tmp/toyos_fix53_layout_loader.bin` -eq 512 || fail layout-loader-size
mkdir -p /tmp/toyos_fix53_tools
cp /tmp/toyos_fix53_mkfat16 /tmp/toyos_fix53_tools/mkfat16.exe
cp /tmp/toyos_fix53_mklayout /tmp/toyos_fix53_tools/mklayout.exe
cp /tmp/toyos_fix53_layoutcheck /tmp/toyos_fix53_tools/layoutcheck.exe
./check_fix53_layout_variants.sh /tmp/toyos_fix53_tools /tmp/toyos_fix53_layout_loader.bin >/dev/null
if [ -f build/disk.img ] && [ -x build/layoutcheck.exe ]; then build/layoutcheck.exe build/disk.img; fi
echo 'FIX53-LAYOUT CHECK: PASS'
