#!/bin/sh
set -eu
# v65.11: AUTOSTART.SH exceeds FAT 8.3, so mkfat16 must emit one LFN entry
# followed by the reserved short alias AUTOST~1.SH.
grep -Fq 'AUTOST~1SH ' tools/mkfat16.c
grep -Fq 'lfn[11]=0x0fu' tools/mkfat16.c
grep -Fq 'short_checksum(short_name)' tools/mkfat16.c
grep -Fq 'strcmp(in,"AUTOSTART.SH")==0' tools/mkfat16.c
grep -Fq 'memcpy(out,"AUTOST~1SH ",11)' tools/fat16check.c
grep -Fq 'mem_copy(out,alias,11)' src/kernel.c
grep -Fq 'AUTOSTART.SH' src/path.c
echo 'check97: PASS'
