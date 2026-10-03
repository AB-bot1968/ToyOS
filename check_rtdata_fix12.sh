#!/bin/sh
set -eu
K=src/kernel.c; S=src/user_shell.c; R=src/rt_sensor_diag.c
grep -q '#define SYS_RT_DATA        50u' "$K"
grep -q '#define SYS_RT_DATA         50u' "$S"
grep -q '#define SYS_RT_DATA 50u' "$R"
grep -q 'RT_DATA_MAX       255u' "$K"
grep -q 'rt_data_clear(slot)' "$K"
grep -q 'rt_data_clear(id)' "$K"
grep -q 'rtdata SLOT1' "$S"
grep -q 'field_u(data,&n,"D8",v\[12\])' "$R"
! grep -q 'SYS_CONSOLE' "$R"
mkdir -p /tmp/toyos-fix12-check
gcc -m32 -ffreestanding -fno-pie -fno-stack-protector -fno-builtin -O2 -c "$K" -o /tmp/toyos-fix12-check/kernel.o
gcc -m32 -ffreestanding -fno-pie -fno-stack-protector -fno-builtin -O2 -c "$S" -o /tmp/toyos-fix12-check/shell.o
gcc -m32 -ffreestanding -fno-pie -fno-stack-protector -fno-builtin -O2 -c "$R" -o /tmp/toyos-fix12-check/sensor.o
echo 'PASS: FIX12 RTDATA snapshot ABI and SENSOR publish'
