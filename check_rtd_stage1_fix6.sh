#!/bin/sh
set -eu

# FIX7 changed the last-RT prompt sentinel from the historical value 2 to 3.
# Value 2 now means only an RT quantum return; value 3 means the last detached
# RT task exited and readline() must repaint toy0> exactly once.
grep -q 'f->eax=3u;' src/kernel.c
grep -q 'if(r==2u)continue;' src/user_shell.c
grep -q 'if(r==3u){put("toy0> ");continue;}' src/user_shell.c
printf '%s\n' 'RTD stage1 FIX6 prompt checks passed (FIX7-compatible)'
