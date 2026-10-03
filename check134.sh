#!/bin/sh
set -eu
fail(){ echo "FAIL: $1" >&2; exit 1; }

test -f src/rt_jitter_diag.c || fail 'JITTER diagnostic source missing'
test -f tools/test_rt_jitter.c || fail 'host jitter arithmetic test missing'

grep -q '#define SYS_RT_JITTER_INFO 46u' src/kernel.c || fail 'jitter syscall id missing'
grep -q 'rt_record_release_observation' src/kernel.c || fail 'release observation mechanism missing'
grep -q 'rt_release_interval_ticks' src/kernel.c || fail 'release interval field missing'
grep -q 'rt_release_max_late' src/kernel.c || fail 'release lateness field missing'
grep -q 'rt_job_dispatch_latency' src/kernel.c || fail 'dispatch latency measurement missing'
grep -q 'case SYS_RT_JITTER_INFO' src/kernel.c || fail 'jitter syscall handler missing'
grep -q 'SYS_RT_JITTER_INFO' src/rt_jitter_diag.c || fail 'JITTER EXE does not use jitter syscall'
grep -q 'JITTER.EXE' build.sh || fail 'JITTER EXE1 not in build'
grep -q 'rt-jitter' src/user_shell.c || fail 'shell jitter command missing'
grep -q 'ascii_fold' src/user_shell.c || fail 'case-insensitive parser missing'
# All shell command dispatch must use eq()/prefix(), which are ASCII-folded.
grep -q 'if(eq(line,"help"))' src/user_shell.c || fail 'command equality parser regression'
grep -q 'if(prefix(line,"exec RTD.EXE "))' src/user_shell.c || fail 'RTD command parser regression'
grep -q 'if(prefix(line,"exec COMDRV.EXE "))' src/user_shell.c || fail 'COMDRV command parser regression'
grep -q 'if(prefix(line,"exec NETDRV.EXE "))' src/user_shell.c || fail 'NETDRV command parser regression'
grep -q 'if(prefix(line,"exec VGADRV.EXE "))' src/user_shell.c || fail 'VGADRV command parser regression'
grep -q 'if(prefix(line,"exec LOADER.EXE "))' src/user_shell.c || fail 'LOADER command parser regression'

tmpdir=${TMPDIR:-/tmp}
tmp="$tmpdir/toyos_rt_jitter_$$"
trap 'rm -rf "$tmp"' EXIT HUP INT TERM
mkdir -p "$tmp"
cc=${CC:-gcc}
$cc -std=c99 -Wall -Wextra -Werror tools/test_rt_jitter.c -o "$tmp/test_rt_jitter"
"$tmp/test_rt_jitter"
CFLAGS='-m32 -ffreestanding -fno-pie -fno-stack-protector -fno-asynchronous-unwind-tables -fno-unwind-tables -fno-builtin -fno-tree-vectorize -fno-tree-slp-vectorize -mno-sse -mno-sse2 -mno-mmx -mno-80387 -nostdinc -nostdlib'
$cc $CFLAGS -Werror -c src/kernel.c -o "$tmp/kernel.o"
$cc $CFLAGS -Werror -c src/user_shell.c -o "$tmp/user_shell.o"
$cc $CFLAGS -Werror -c src/rtd.c -o "$tmp/rtd.o"
$cc $CFLAGS -Werror -c src/rt_jitter_diag.c -o "$tmp/rt_jitter_diag.o"
$cc $CFLAGS -Werror -c src/rt_sensor.c -o "$tmp/rt_sensor.o"

echo 'CHECK134 PASS: Stage 6.2 jitter measurement, executable diagnostics and case-insensitive command parsing verified'
