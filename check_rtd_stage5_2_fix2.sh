#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
K="$ROOT/src/kernel.c"
R="$ROOT/src/rtd.c"

grep -q "static uint32_t rt_recount_active(void)" "$K"
grep -q "rt_recount_active();" "$K"
grep -q "case SYS_RT_STATUS" "$K"
grep -q "SYS_RT_STATUS     43u" "$R"
grep -q " active=" "$R"
grep -q " free=" "$R"

# Recompute must be based on live RT slot states, not a monotonic counter.
python3 - "$K" <<'PY'
import sys
s=open(sys.argv[1],encoding='utf-8').read()
start=s.index('static uint32_t rt_recount_active(void)')
end=s.index('static int rt_find_current',start)
b=s[start:end]
assert 'TASK_READY' in b and 'TASK_RUNNING' in b and 'TASK_BLOCKED' in b
assert 'TASK_EXIT' not in b and 'TASK_STOPPED' not in b
PY

printf '%s\n' 'RTD Stage 5.2 FIX2 recount/status checks passed'
