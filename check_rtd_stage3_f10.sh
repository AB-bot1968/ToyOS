#!/bin/sh
set -eu
fail=0
ok(){ echo "PASS: $1"; }
bad(){ echo "FAIL: $1"; fail=1; }

grep -q '#define KEY_EVENT_F10 0xf0u' src/kernel.c && ok 'F10 internal event code' || bad 'missing F10 event code'
grep -q 'if(s==0x44){key_push(KEY_EVENT_F10);return;}' src/kernel.c && ok 'PS/2 Set-1 F10 scancode 0x44' || bad 'F10 scancode handler missing'
grep -q 'if(ch==KEY_EVENT_F10).*f->eax=4u' src/kernel.c && ok 'SYS_CONSOLE_READ returns F10 event' || bad 'F10 read result missing'
grep -q '#define SHELL_READ_F10 4u' src/user_shell.c && ok 'shell F10 result code' || bad 'shell F10 result missing'
grep -q 'shell_f10_event=1u' src/user_shell.c && ok 'readline reports F10 to shell loop' || bad 'readline F10 event missing'
grep -q 'RTD: task prepared' src/user_shell.c && ok 'RTD command is queued instead of started immediately' || bad 'RTD queueing message missing'
grep -q 'pending_rt_args' src/user_shell.c && ok 'pending RT argument storage' || bad 'pending RT storage missing'
if awk '/static void exec_rtd_command/{infn=1;next} /static void rt_launch_pending/{infn=0} infn && /sys_exec_args\(\"\/RTD.EXE\"/{found=1} END{exit(found?0:1)}' src/user_shell.c; then bad 'RTD still starts immediately from exec_rtd_command'; else ok 'exec_rtd_command does not launch RTD immediately'; fi
grep -q 'rt_launch_pending' src/user_shell.c && ok 'F10 launch path exists' || bad 'F10 launch path missing'
grep -q 'if(shell_f10_event){rt_launch_pending();continue;}' src/user_shell.c && ok 'shell loop launches pending tasks on F10' || bad 'shell F10 launch integration missing'
if [ "$fail" -ne 0 ]; then exit 1; fi
echo 'RTD Stage 3 F10 checks passed'
grep -q '__attribute__((section(".userdata"),used)) static uint32_t shell_f10_event=0u' src/user_shell.c && ok 'F10 state is user-mapped data' || bad 'F10 state remains kernel-only data'
grep -q 'paging_mark_user_rw((uint32_t)__user_data_start,(uint32_t)__user_data_end)' src/kernel.c && ok 'user data is mapped RW for shell' || bad 'shell user data mapping missing'
