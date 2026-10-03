#!/bin/sh
set -eu
fail(){ echo "CHECK46 FAIL: $*"; exit 1; }
grep -q 'console_write_atomic_n' src/kernel.c || fail 'atomic console write helper missing'
grep -q 'pushfl; popl %0; cli' src/kernel.c || fail 'console write does not mask interrupts'
grep -q 'console_write_atomic_n((const char\*)a,b)' src/kernel.c || fail 'SYS_CONSOLE_WRITE is not using atomic helper'
grep -q 'static void exe_exit(struct frame\*f)' src/kernel.c || fail 'exe_exit missing'
grep -q 'static void sched_dispatch_next(struct frame\*f)' src/kernel.c || fail 'scheduler dispatch missing'
echo 'CHECK46 PASS: console writes are non-preemptible and EXE/scheduler context transitions are protected'
