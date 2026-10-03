#!/bin/sh
set -eu
fail(){ echo "FAIL: $1"; exit 1; }
grep -q 'RTSTAT WATCH  (time: 10-ms ticks)' src/user_shell.c || fail 'WATCH table title missing'
grep -q 'SLOT TASK         JOBS   INT  JIT  DSP  CPU  WALL MISS SKIP' src/user_shell.c || fail 'RTSTAT table header missing'
grep -q 'SEQ    INT  JIT  DSP  CPU  WALL MISS SKIP' src/user_shell.c || fail 'DUMP table header missing'
grep -Fq 'm->ring[i][7]=m->skips' src/kernel.c || fail 'DUMP cumulative SKIP snapshot missing'
test "$(grep -n 't->rt_skipped_releases+=skipped' src/kernel.c | tail -1 | cut -d: -f1)" -lt "$(grep -n 'rt_stats_record(t,now' src/kernel.c | tail -1 | cut -d: -f1)" || fail 'skip accounting still recorded late'
CFLAGS='-Os -ffreestanding -fno-pie -fno-stack-protector -fno-asynchronous-unwind-tables -fno-unwind-tables -fno-builtin -fno-tree-vectorize -fno-tree-slp-vectorize -mno-sse -mno-sse2 -mno-mmx -mno-80387 -nostdinc -nostdlib -m32'
mkdir -p build/check_fix8
gcc $CFLAGS -c src/kernel.c -o build/check_fix8/kernel.o
gcc $CFLAGS -c src/user_shell.c -o build/check_fix8/user_shell.o
echo 'PASS: RTSTAT table output and DUMP accounting fix'
