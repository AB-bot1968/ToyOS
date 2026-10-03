#!/bin/sh
set -eu
CC=${CC:-gcc}
CFLAGS='-m32 -std=c99 -ffreestanding -fno-builtin -fno-stack-protector -Wall -Wextra -Werror -fsyntax-only'
$CC $CFLAGS -Isrc -Isrc/include src/kernel.c
$CC $CFLAGS -Isrc -Isrc/include src/user_shell.c
$CC $CFLAGS -Isrc -Isrc/include src/rtd.c
$CC $CFLAGS -Isrc -Isrc/include src/rt_sensor_diag.c

grep -q 'if(verbose){' src/rt_sensor_diag.c
grep -q 'rt_shell_frame_valid=1u' src/kernel.c
grep -q 'if(rt_shell_waiting){rt_shell_frame\[7\]=2u' src/kernel.c
# Background SENSOR binaries must not contain unconditional startup/stop output.
if grep -q 'put(": started period=")' src/rt_sensor_diag.c && ! grep -B4 -A2 'put(": started period=")' src/rt_sensor_diag.c | grep -q 'if(verbose)'; then
  echo 'CHECK141 FAIL: SENSOR startup output is not verbose-gated' >&2; exit 1
fi
printf '%s\n' 'CHECK141 PASS: background SENSOR console silence and shell/RT context handoff verified'
