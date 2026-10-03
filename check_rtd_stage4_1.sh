#!/bin/sh
set -eu

test -f src/rt_priority.h
test -f tools/test_rt_priority.c

grep -q 'static inline int rt_priority_compare' src/rt_priority.h
grep -q 'if(a>b)return 1;' src/rt_priority.h
grep -q 'if(a<b)return -1;' src/rt_priority.h
grep -q 'return 0;' src/rt_priority.h

# Stage 4.1 comparator contract remains present after Stage 4.2 arbitration wiring.
# Unit-test the exact comparator header used by kernel.c.
cc=${CC:-gcc}
tmpdir=${TMPDIR:-/tmp}
tmp="$tmpdir/toyos_rt_priority_$$"
trap 'rm -rf "$tmp"' EXIT HUP INT TERM
mkdir -p "$tmp"
$cc -std=c99 -Wall -Wextra -Werror tools/test_rt_priority.c -o "$tmp/test_rt_priority"
"$tmp/test_rt_priority"

echo 'RTD Stage 4.1 priority comparator checks passed'
