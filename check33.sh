#!/bin/sh
set -eu
fail(){ echo "FAIL: $1" >&2; exit 1; }
grep -q '#define PIT_HZ 50u' src/kernel.c || fail 'PIT_HZ 50 missing'
grep -q '#define RT_TIME_HZ 100u' src/kernel.c || fail '100 Hz RT timebase missing'
grep -q '1193182u+(RT_TIME_HZ/2u)' src/kernel.c || fail 'rounded 100 Hz PIT divisor missing'
grep -q 'rt_time_ticks++' src/kernel.c || fail 'RT time counter path missing'
grep -q 'timer_ticks++' src/kernel.c || fail '50 Hz logical system counter missing'
grep -q 'PIT(50Hz sys, 100Hz RT)' src/kernel.c || fail 'dual-rate init banner missing'
grep -q 'pit-test' src/user_shell.c || fail 'pit-test command missing'
grep -q 'waiting for 3 timer ticks' src/user_shell.c || fail 'pit-test wait missing'
grep -q 'SYS_TIMER_GET' src/user_shell.c || fail 'pit-test syscall path missing'
grep -q 'execq' README.md || fail 'existing execq documentation missing'
grep -q '50 Hz' README.md || fail 'v33 README missing'
grep -q '50 Hz' DESIGN.md || fail 'v33 design missing'
test -f VERIFICATION_V33_RU.md || fail 'v33 verification doc missing'
# Existing queue ABI/names must remain present.
grep -q '#define SYS_EXEC_QUEUE  15u' src/kernel.c || fail 'syscall 15 changed'
grep -q '#define SYS_QUEUE_STOP  16u' src/kernel.c || fail 'syscall 16 changed'
grep -q 'QUEUE_MAX_DEPTH 4u' src/kernel.c || fail 'queue depth changed'
python3 - <<'PY'
from pathlib import Path
s=Path('src/kernel.c').read_text()
assert 'static void pit_init(void){uint32_t div=(1193182u+(RT_TIME_HZ/2u))/RT_TIME_HZ;' in s
assert 'if(n==32){rt_time_ticks++;rt_time_phase^=1u;' in s
print('PASS: logical 50 Hz system timer preserved; 100 Hz RT timebase added')
PY
echo '=== CHECK33 PASS ==='
