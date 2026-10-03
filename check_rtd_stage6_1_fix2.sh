#!/bin/sh
set -eu
fail(){ echo "FAIL: $1" >&2; exit 1; }
test -f src/kernel.c || fail 'kernel source missing'
test -f src/rt_sensor_diag.c || fail 'diagnostic SENSOR source missing'
python3 - <<'PY'
from pathlib import Path
k=Path('src/kernel.c').read_text()
s=k[k.index('case SYS_CONSOLE_READ:'):k.index('case SYS_CONSOLE_POLL:', k.index('case SYS_CONSOLE_READ:'))]
assert 'int rid=rt_pick_ready();' in s
assert 'if(rid<0)rid=rt_pick_ready_missed();' in s
chunk=k[k.index('static void sched_irq_tick(struct frame*f){'):k.index('\nstatic void sched_block(struct frame*f){')]
assert 'if((f->cs&3u)==0u){' in chunk
assert 'rt_release_jobs(now);' in chunk
d=Path('src/rt_sensor_diag.c').read_text()
assert '#define SYS_RT_TIME_GET  45u' in d
assert 'now=sc(SYS_RT_TIME_GET,0,0,0)' in d
print('kernel shell fallback checks: PASS')
print('diagnostic RT clock checks: PASS')
PY
TMP=${TMPDIR:-/tmp}/toyos_rt_stage61_fix2_$$; trap 'rm -rf "$TMP"' EXIT HUP INT TERM
mkdir -p "$TMP"
C='-m32 -ffreestanding -fno-pic -fno-stack-protector -fno-builtin -Os -Wall -Wextra -nostdinc -nostdlib'
gcc $C -c src/kernel.c -o "$TMP/kernel.o"
gcc $C -DRT_SENSOR_DIAG_ID=1 -c src/rt_sensor_diag.c -o "$TMP/s1.o"
gcc $C -DRT_SENSOR_DIAG_ID=2 -c src/rt_sensor_diag.c -o "$TMP/s2.o"
printf '%s\n' 'RTD Stage 6.1 FIX2 focused checks passed'
