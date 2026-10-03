#!/bin/sh
set -eu

# Stage 5.1 FIX1: the current job's absolute release/deadline must remain
# immutable after a deadline miss. Only a miss flag/counter may change until
# SYS_RT_WAIT completes the current job.

test -f src/kernel.c
test -f src/rt_deadline_diag.c

grep -q 'uint32_t rt_job_missed;' src/kernel.c
grep -q 't->rt_job_missed=0u;' src/kernel.c

grep -q 'if(!t->rt_job_missed)' src/kernel.c
grep -q 't->rt_job_missed=1u;' src/kernel.c

grep -q 't->rt_missed_deadlines++;' src/kernel.c

grep -q 'if(t->rt_job_missed||rt_deadline_reached(now,t->rt_deadline_tick))' src/kernel.c

grep -q 't->rt_job_active=0u;t->rt_job_missed=0u;' src/kernel.c

# Extract the active-job deadline block and assert that it does not roll the
# current job to the next release or block it immediately at the deadline.
block="$(awk '/if\(t->rt_job_active&&rt_deadline_reached\(now,t->rt_deadline_tick\)\)\{/{on=1} on{print} on&&/^        \}/{exit}' src/kernel.c)"
printf '%s\n' "$block" | grep -q 't->rt_job_missed=1u;'
if printf '%s\n' "$block" | grep -q 't->rt_job_active=0u'; then
    echo 'FAIL: active deadline path deactivates current job' >&2
    exit 1
fi
if printf '%s\n' "$block" | grep -q 'rt_advance_release'; then
    echo 'FAIL: active deadline path advances release' >&2
    exit 1
fi
if printf '%s\n' "$block" | grep -q 'rt_deadline_tick='; then
    echo 'FAIL: active deadline path rewrites absolute deadline' >&2
    exit 1
fi
if printf '%s\n' "$block" | grep -q 'TASK_BLOCKED'; then
    echo 'FAIL: active deadline path blocks current job' >&2
    exit 1
fi

grep -q 'missed=' src/rt_deadline_diag.c

grep -q 'check_rtd_stage5_1_fix1.sh' build.sh

cc=${CC:-gcc}
tmpdir=${TMPDIR:-/tmp}
tmp="$tmpdir/toyos_rt_stage51_fix1_$$"
trap 'rm -rf "$tmp"' EXIT HUP INT TERM
mkdir -p "$tmp"
$cc -std=c99 -Wall -Wextra -Werror tools/test_rt_deadline.c -o "$tmp/test_rt_deadline"
"$tmp/test_rt_deadline"
$cc -m32 -ffreestanding -fno-pic -fno-stack-protector -fno-builtin -Os -Wall -Wextra -nostdinc -nostdlib -c src/kernel.c -o "$tmp/kernel.o"
$cc -m32 -ffreestanding -fno-pic -fno-stack-protector -fno-builtin -Os -Wall -Wextra -nostdinc -nostdlib -c src/rt_deadline_diag.c -o "$tmp/rt_deadline_diag.o"
echo 'RTD Stage 5.1 FIX1 absolute-deadline immutability checks passed'
