#!/bin/sh
set -eu
fail(){ echo "CHECK FAILED: $1" >&2; exit 1; }
B=build
[ -f "$B/RTD.EXE" ] || fail RTD.EXE
[ -f "$B/SENSOR.EXE" ] || fail SENSOR.EXE
[ -f "$B/kernel.bin" ] || fail kernel.bin
[ -f "$B/disk.img" ] || fail disk.img
"$B/fat16check.exe" "$B/disk.img" RTD.EXE >/dev/null || fail "RTD.EXE root entry"
"$B/fat16check.exe" "$B/disk.img" SENSOR.EXE >/dev/null || fail "SENSOR.EXE root entry"
grep -q '#define SYS_RT_START[[:space:]]*38u' src/kernel.c || fail SYS_RT_START
 grep -q 'exec RTD.EXE SENSOR.EXE PERIOD_MS DEADLINE_MS PRIORITY' src/user_shell.c || fail shell-usage
grep -q 'rt_start_task' src/kernel.c || fail rt-start-kernel
grep -q 'rt_background_active' src/kernel.c || fail detached-state
grep -q 'rt_period_ms' src/kernel.c || fail rt-metadata
grep -q 'exec RTD.EXE ' src/user_shell.c || fail shell-command
grep -q 'SYS_RT_START' src/rtd.c || fail rtd-syscall
grep -q 'SYS_EXEC_ARG' src/rtd.c || fail rtd-arg-abi
grep -q 'ordinary EXE1' src/rt_sensor.c || fail ordinary-sensor
grep -q 'SYS_CONSOLE_POLL 37u' src/rt_sensor.c || fail sensor-esc-poll
grep -q 'key==0x1bu' src/rt_sensor.c || fail sensor-esc-check
grep -q 'rt_stop_requested' src/kernel.c || fail rt-stop-request
grep -q 'if(s==0x01)' src/kernel.c || fail keyboard-esc
# ABI/header sanity: EXE1 magic and load address for both new binaries.
for f in RTD SENSOR; do
  [ "`od -An -t x4 -N 4 "$B/$f.EXE" | tr -d ' \\n'`" = 31455845 ] || fail "$f EXE1 magic"
  [ "`od -An -t x4 -j 4 -N 4 "$B/$f.EXE" | tr -d ' \\n'`" = 00100000 ] || fail "$f entry"
done
[ `wc -c < "$B/kernel.bin"` -le 131072 ] || fail "kernel > 128KiB"
echo "RTD stage 1 checks passed"
