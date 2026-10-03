#!/bin/sh
set -eu

test -f src/rt_deadline.h
test -f src/rt_period_skip_diag.c
test -f tools/test_rt_period_release.c

grep -q 'rt_next_release_after' src/rt_deadline.h
grep -q 'rt_skipped_releases' src/kernel.c
grep -q 'rt_job_sequence' src/kernel.c
grep -Eq '#define SYS_RT_JOB_INFO[[:space:]]+44u' src/kernel.c
grep -q 'case SYS_RT_JOB_INFO' src/kernel.c
grep -q 't->rt_skipped_releases+=skipped' src/kernel.c
grep -q 'if(next==now)' src/kernel.c
grep -q 'rt_begin_job(t,next)' src/kernel.c
grep -q 'build/RTDSKIP.EXE=RTDSKIP.EXE' build.sh
grep -q 'rt_period_skip_diag.c' build.sh
grep -q 'check_rtd_stage5_3.sh' build.sh

grep -q 'Stage 5.3' changes/RTD/Stage5/Stage5_3/RELEASE_NOTES_RU.md

grep -q 'no-backlog' changes/RTD/Stage5/Stage5_3/RELEASE_NOTES_RU.md

grep -q 'skipped_releases' changes/RTD/Stage5/Stage5_3/VERIFICATION_RU.md

cc=${CC:-gcc}
tmpdir=${TMPDIR:-/tmp}
tmp="$tmpdir/toyos_rt_stage53_$$"
trap 'rm -rf "$tmp"' EXIT HUP INT TERM
mkdir -p "$tmp"
$cc -std=c99 -Wall -Wextra -Werror tools/test_rt_period_release.c -o "$tmp/test_rt_period_release"
"$tmp/test_rt_period_release"
$cc -m32 -ffreestanding -fno-pic -fno-stack-protector -fno-builtin -Os -Wall -Wextra -nostdinc -nostdlib -c src/kernel.c -o "$tmp/kernel.o"
$cc -m32 -ffreestanding -fno-pic -fno-stack-protector -fno-builtin -Os -Wall -Wextra -nostdinc -nostdlib -c src/rt_period_skip_diag.c -o "$tmp/rt_period_skip_diag.o"

echo 'RTD Stage 5.3 periodic release/no-backlog checks passed'
