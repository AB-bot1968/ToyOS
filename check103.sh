#!/bin/sh
set -eu
printf '[check103] v66 UDP telemetry static regression\n'
grep -q '#define SYS_CONSOLE_AT     36u' src/kernel.c
grep -q 'case SYS_CONSOLE_AT:' src/kernel.c
grep -q 'ip_send_udp' src/netdrv.c
grep -q 'parse_ipv4_udp' src/netdrv.c
grep -q 'TELEMETRY_PORT 5000u' src/netdrv.c
grep -q 'TELEMETRY SLAVE1' src/netdrv.c
grep -q 'TELEMETRY MASTER' src/netdrv.c
grep -q 'SYS_CONSOLE_AT' src/user_shell.c
grep -q 'udp-test' src/user_shell.c
printf '[check103] PASS\n'
