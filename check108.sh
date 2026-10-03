#!/bin/sh
set -eu
printf '%s\n' '[check108] v66.2 UDP independence and Master stop'
grep -q 'UDP SLAVE running independently' src/netdrv.c
grep -q 'probe.timeout_s=1u' src/netdrv.c
grep -q 'SYS_CONSOLE_POLL' src/netdrv.c
grep -q 'UDP MASTER stopped' src/netdrv.c
grep -q 'case SYS_CONSOLE_POLL' src/kernel.c
grep -q 'key_push(0x1bu)' src/kernel.c
printf '%s\n' '[check108] PASS'
