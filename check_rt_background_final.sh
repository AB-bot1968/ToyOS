#!/bin/sh
set -eu
fail(){ echo "FAIL: $1" >&2; exit 1; }
test -f src/rtd.c || fail 'RTD source missing'
test -f src/rt_sensor_diag.c || fail 'SENSOR diagnostic source missing'
test -f src/user_shell.c || fail 'shell source missing'
python3 - <<'PY'
from pathlib import Path
r=Path('src/rtd.c').read_text()
assert 'verbose=0' not in r.lower(), 'RTD verbose state remains'
assert 'tok[0]==\'v\'' not in r and 'verbose token' not in r.lower(), 'legacy verbose parser remains'
assert "tok[0]=='q'" not in r and "tok[0]=='v'" not in r, 'legacy quiet/verbose parser remains'
s=Path('src/rt_sensor_diag.c').read_text()
assert 'SYS_CONSOLE_WRITE' not in s, 'SENSOR must not write to interactive console'
assert 'SYS_CONSOLE_POLL' not in s, 'SENSOR must not consume shell keyboard input'
assert 'SYS_RT_WAIT' in s, 'SENSOR periodic wait missing'
sh=Path('src/user_shell.c').read_text()
assert 'next_rt=sys_rt_time_get()+100u;' in sh and '(uint32_t)(cur-(next_rt-100u))>=100u' in sh, 'rtstat watch must refresh every 1 second (100 RT ticks)'
assert "pending_rt_args[i][base+k+1u]='q'" not in sh, 'F10 must not append private quiet option'
print('check_rt_background_final: PASS')
PY
