#!/bin/sh
set -eu
fail(){ echo "FAIL: $1" >&2; exit 1; }
test -f src/kernel.c || fail 'kernel source missing'
grep -q 'Stage 6.1 FIX1: never context-switch' src/kernel.c || fail 'RT kernel-frame guard comment missing'
python3 - <<'PY'
from pathlib import Path
k=Path('src/kernel.c').read_text()
start=k.index('static void sched_irq_tick(struct frame*f){')
end=k.index('\nstatic void sched_block(struct frame*f){',start)
chunk=k[start:end]
guard=chunk.index('if((f->cs&3u)==0u){')
assert 'rt_release_jobs(now);' in chunk
assert 'mem_copy(t->words,(const void*)f' in chunk
# FIX1 invariant: Ring-0 cannot copy its transient frame into a user context.
ring0=chunk[guard:chunk.index('int current;',guard)]
assert 'mem_copy(' not in ring0
print('Stage 6.1 FIX1 kernel-mode context-switch guard checks passed')
PY
TMP=${TMPDIR:-/tmp}/toyos_rt_stage61_fix1_$$; trap 'rm -rf "$TMP"' EXIT HUP INT TERM
mkdir -p "$TMP"
C='-m32 -ffreestanding -fno-pic -fno-stack-protector -fno-builtin -Os -Wall -Wextra -nostdinc -nostdlib'
gcc $C -c src/kernel.c -o "$TMP/kernel.o"
gcc $C -c src/rt_sensor_diag.c -o "$TMP/rt_sensor_diag.o"
gcc $C -c src/rt_sensor.c -o "$TMP/rt_sensor.o"
printf '%s\n' 'RTD Stage 6.1 FIX1 focused checks passed'
