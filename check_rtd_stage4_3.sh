#!/bin/sh
set -eu

test -f src/rt_sensor_diag.c

grep -q 'RT_SENSOR_DIAG_ID=1' build.sh
grep -q 'RT_SENSOR_DIAG_ID=2' build.sh
grep -q 'build/SENSOR1.EXE=SENSOR1.EXE' build.sh
grep -q 'build/SENSOR2.EXE=SENSOR2.EXE' build.sh

grep -q 'SENSOR1: started ordinary EXE1' src/rt_sensor_diag.c
grep -q 'SENSOR1: tick' src/rt_sensor_diag.c
grep -q 'SENSOR1: ESC -> stopped' src/rt_sensor_diag.c
grep -q 'SENSOR2: started ordinary EXE1' src/rt_sensor_diag.c
grep -q 'SENSOR2: tick' src/rt_sensor_diag.c
grep -q 'SENSOR2: ESC -> stopped' src/rt_sensor_diag.c

grep -q 'rt_priority_compare' src/kernel.c
grep -q 'rt_pick_ready' src/kernel.c

grep -q 'make_exe1 build/rt_sensor1.raw build/SENSOR1.EXE 0' build.sh
grep -q 'make_exe1 build/rt_sensor2.raw build/SENSOR2.EXE 0' build.sh

cc=${CC:-gcc}
tmpdir=${TMPDIR:-/tmp}
tmp="$tmpdir/toyos_rt_stage43_$$"
trap 'rm -rf "$tmp"' EXIT HUP INT TERM
mkdir -p "$tmp"
$cc -std=c99 -Wall -Wextra -Werror tools/test_rtd_stage4_3.c -o "$tmp/test_rtd_stage4_3"
"$tmp/test_rtd_stage4_3"

echo 'RTD Stage 4.3 diagnostic SENSOR1/SENSOR2 checks passed'
