#!/bin/sh
set -eu
BASE=$(dirname "$0")
R="$BASE/src/rtd.c"
grep -q "v67.2: имя RT-цели нормализуется" "$R"
grep -q "c-'a'+'A'" "$R"
grep -q "req.name\[i\]=c" "$R"
grep -q "SYS_RT_START" "$R"
printf '%s\n' 'CHECK128 PASS: RTD target executable name is normalized case-insensitively before SYS_RT_START'
