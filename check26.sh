#!/bin/sh
# FIX53 supersedes fixed kernel-slot/FAT offset assumptions.
set -eu
grep -q 'fat_mount(layout_word(9))' src/kernel.c
grep -q 'LAYOUT_GAP_SECTORS' build.sh
! grep -q '^#define FAT_LBA 512u' src/kernel.c
echo 'PASS: kernel/FAT placement is descriptor-driven'
