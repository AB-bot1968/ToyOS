#!/bin/sh
set -eu
fail=0
ok(){ echo "PASS: $1"; }
bad(){ echo "FAIL: $1"; fail=1; }

if grep -q 'for(i=0;i<sizeof(req);i++)((unsigned char\*)\&req)\[i\]=0;' src/rtd.c && \
   grep -n 'p=spec;' src/rtd.c >/dev/null; then ok 'RTD initializes request before parsing numeric fields'; else bad 'RTD request initialization order'; fi
if grep -q 'SYS_CONSOLE_POLL 37u' src/rt_sensor.c && grep -q 'key==0x1bu' src/rt_sensor.c; then ok 'SENSOR ESC polling'; else bad 'SENSOR ESC polling'; fi
if grep -q 'rt_stop_requested' src/kernel.c && grep -q 'if(s==0x01)' src/kernel.c; then ok 'kernel ESC stop request'; else bad 'kernel ESC stop request'; fi
if grep -q 'rt_stop_requested=0;' src/kernel.c && grep -q 'rt_background_active=0;' src/kernel.c; then ok 'RT stop cleanup'; else bad 'RT stop cleanup'; fi
if grep -q 'user_cstr(p)' src/kernel.c | grep -q . 2>/dev/null; then bad 'old binary-structure user_cstr check remains'; else ok 'no old request user_cstr check'; fi
[ "$fail" -eq 0 ]
