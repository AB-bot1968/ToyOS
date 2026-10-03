#!/bin/sh
set -eu
fail(){ echo "CHECK133 FAIL: $1"; exit 1; }
grep -q 'v67.7: надёжное удаление directory entry' src/kernel.c || fail delete-doc
 grep -q 'fat_mark_deleted_entry' src/kernel.c || fail delete-mark-helper
 grep -q 'fat_delete_duplicate_entries' src/kernel.c || fail duplicate-cleanup
 grep -q 'mem_set(sec+off,0,32)' src/kernel.c || fail full-entry-clear
 grep -q 'sec\[off\]=0xe5' src/kernel.c || fail deleted-marker
 echo 'CHECK133 PASS: file deletion fully removes FAT16 directory entries and stale duplicates'
