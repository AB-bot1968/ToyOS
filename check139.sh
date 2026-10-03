#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
CC=${CC:-gcc}
CFLAGS="-m32 -std=c99 -ffreestanding -fno-builtin -fno-stack-protector -Wall -Wextra -Werror"
for f in kernel.c user_shell.c rt_sensor.c rtd.c; do
  "$CC" $CFLAGS -fsyntax-only "$ROOT/src/$f"
done
# Background SENSOR must not print unsolicited shell text.
if grep -Eq 'put\("SENSOR: (started|tick|deadline miss|ESC)' "$ROOT/src/rt_sensor.c"; then
  echo "CHECK139 FAIL: SENSOR still writes unsolicited console output" >&2
  exit 1
fi
# F10 background launcher must request quiet RTD mode.
grep -q "pending_rt_args\[i\]\[base+k+1u\]='q'" "$ROOT/src/user_shell.c"
# RTD must accept the private quiet option and suppress launcher chatter.
grep -q 'quiet=0' "$ROOT/src/rtd.c"
grep -q 'if(!quiet){put("RTD: started' "$ROOT/src/rtd.c"
# The shell must retain the rtstat command dispatcher.
grep -q 'if(eq(line,"rtstat"))' "$ROOT/src/user_shell.c"
grep -q 'if(prefix(line,"rtstat "))' "$ROOT/src/user_shell.c"
echo "CHECK139 PASS: background RT console race fix verified"
