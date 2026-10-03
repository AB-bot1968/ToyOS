#!/bin/sh
set -eu
python3 - <<'PY'
from pathlib import Path
sensor=Path('src/rt_sensor.c').read_text()
rtd=Path('src/rtd.c').read_text()
sh=Path('src/user_shell.c').read_text()
assert 'SYS_CONSOLE_POLL' not in sensor
assert 'SYS_RT_WAIT' in sensor
assert 'RTD: started ' not in rtd
assert 'put("RTD: started ");put(name);put(" ");put(spec);put("\\n");' in sh
assert 'if(eq(s,"slot1"))return 0u;' in sh
assert 'if(eq(s,"slot4"))return 3u;' in sh
assert 'next_rt=sys_rt_time_get()+100u;' in sh
assert '(uint32_t)(cur-(next_rt-100u))>=100u' in sh
print('check_rt_background_fix6: PASS')
PY
