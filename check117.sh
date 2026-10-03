#!/bin/sh
set -eu
printf '%s\n' 'check117: проверка абсолютных путей NETDRV'
grep -q 'const char bin_name\[\]="/BIN/NETDRV.EXE"' src/user_shell.c
grep -q 'const char name\[\]="/NETDRV.EXE"' src/user_shell.c
grep -q 'v66.7' src/user_shell.c
printf '%s\n' 'check117: PASS'
