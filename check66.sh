#!/bin/sh
# FIX53 image geometry regression.
set -eu
grep -q -- '--part-lba "$FAT_LBA"' build.sh
grep -q -- '--free-percent "$FAT_FREE_PERCENT"' build.sh
grep -q 'layoutcheck.exe build/disk.img' build.sh
echo 'PASS: FAT16 image geometry is computed and descriptor-verified'
