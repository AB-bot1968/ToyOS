#!/bin/sh
set -eu
fail(){ echo "FAIL: $1" >&2; exit 1; }
test -f src/kernel.c || fail 'kernel source missing'
test -f src/rt_sensor.c || fail 'SENSOR source missing'
python3 - <<'PY'
from pathlib import Path
k=Path('src/kernel.c').read_text()
start=k.index('static void sched_irq_tick(struct frame*f){')
end=k.index('\nstatic void sched_block(struct frame*f){', start)
chunk=k[start:end]
mark=chunk.index('Stage 6.1 FIX3:')
guard=chunk.index('if((f->cs&3u)==0u){',mark)
release=chunk.index('rt_release_jobs(now);',guard)
ret=chunk.index('return;',release)
assert guard < release < ret, 'Ring-0 IRQ0 must progress RT release/deadline before returning'
pre=chunk[guard:ret]
assert 'load_cr3(' not in pre and 'mem_copy(' not in pre, 'Ring-0 branch must not context-switch or copy kernel frame'
assert 'uint32_t now=rt_time_ticks;' in chunk, 'RT clock missing'
assert chunk.count('rt_release_jobs(now);') >= 2, 'Ring-3 and Ring-0 release paths missing'
s=Path('src/rt_sensor.c').read_text()
assert 'SYS_RT_WAIT' in s and 'SENSOR: tick\\n' in s, 'ordinary SENSOR periodic path missing'
print('Stage 6.1 FIX3 Ring-0 release progression checks passed')
PY
TMP=${TMPDIR:-/tmp}/toyos_rt_stage61_fix3_$$; trap 'rm -rf "$TMP"' EXIT HUP INT TERM
mkdir -p "$TMP"
C='-m32 -ffreestanding -fno-pic -fno-stack-protector -fno-builtin -Os -Wall -Wextra -nostdinc -nostdlib'
for f in kernel.c user_shell.c rtd.c rt_sensor.c rt_sensor_diag.c rt_deadline_diag.c rt_deadline_miss_diag.c rt_period_skip_diag.c rt_timebase_diag.c; do
  gcc $C -c "src/$f" -o "$TMP/${f%.c}.o" || exit 1
done
printf '%s\n' 'RTD Stage 6.1 FIX3 compile checks passed'
