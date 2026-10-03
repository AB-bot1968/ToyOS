#!/bin/sh
set -eu
fail(){ echo "FAIL: $1"; exit 1; }
pass(){ echo "PASS: $1"; }
grep -q 'if(!rt_shell_frame_valid||!rt_shell_waiting)return;' src/kernel.c || fail 'shell switch requires waiting shell context'
grep -q 'rt_shell_frame\[7\]=2u;' src/kernel.c || fail 'RT-to-shell sentinel missing'
python3 - <<'PY2'
from pathlib import Path
s=Path('src/kernel.c').read_text()
a=s.index('static void rt_switch_to_shell')
b=s.index('static void rt_wait',a)
blk=s[a:b]
assert blk.index('rt_shell_frame[7]=2u;') < blk.index('load_cr3(sched_saved_shell_cr3);')
print('PASS: sentinel is applied before shell CR3 restore')
PY2
grep -q 'if(!sched_active&&!rt_background_active)return;' src/kernel.c || fail 'existing scheduler guard changed'
grep -q 'if(r==2u)continue;' src/user_shell.c || fail 'quantum return must not repaint prompt'
grep -q 'if(r==3u){put("toy0> ");continue;}' src/user_shell.c || fail 'last RT exit must repaint prompt'
test -s changes/RTD/Stage3_FIX3/RELEASE_NOTES_RU.md || fail 'release notes missing'
test -s changes/RTD/Stage3_FIX3/TEST_PLAN_RU.md || fail 'test plan missing'
test -s changes/RTD/Stage3_FIX3/VERIFICATION_RU.md || fail 'verification missing'
test -s changes/RTD/Stage3_FIX3/STAGE3_FIX3.patch || fail 'patch missing'
pass 'RTD stage3 FIX3 checks'
