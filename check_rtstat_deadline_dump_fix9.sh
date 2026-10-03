#!/bin/sh
set -eu
K=src/kernel.c
H=src/rt_deadline.h
# Deadline equality is on-time: miss only strictly after absolute deadline.
grep -q 'rt_deadline_before(t->rt_deadline_tick,now)' "$K"
! grep -q 'rt_deadline_reached(now,t->rt_deadline_tick)' "$K"
# DUMP snapshots use cumulative MISS/SKIP, same semantics as RTSTAT summary.
grep -q 'm->ring\[i\]\[6\]=m->misses' "$K"
grep -q 'm->ring\[i\]\[7\]=m->skips' "$K"
cat > /tmp/toyos_rt_deadline_fix9.c <<'C'
#include <assert.h>
#include "src/rt_deadline.h"
int main(void){
 unsigned d=101u;
 assert(!rt_deadline_before(d,100u));
 assert(!rt_deadline_before(d,101u)); /* completion exactly at deadline is on-time */
 assert(rt_deadline_before(d,102u));  /* only strictly late completion misses */
 {unsigned s=99u; assert(rt_next_release_after(101u,101u,1u,&s)==101u&&s==0u);} /* equality is not skipped */
 {unsigned s=0u; assert(rt_next_release_after(101u,102u,1u,&s)==103u&&s==2u);}
 return 0;
}
C
gcc -std=c99 -Wall -Wextra -I. /tmp/toyos_rt_deadline_fix9.c -o /tmp/toyos_rt_deadline_fix9
/tmp/toyos_rt_deadline_fix9
echo 'PASS: FIX9 strict deadline and cumulative DUMP semantics'
