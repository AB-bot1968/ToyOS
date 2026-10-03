#!/bin/sh
set -eu
grep -Fq 'if [ -f resources/AUTOSTART.SH ]; then' build.sh
grep -Fq 'if [ -f build/AUTOSTART.SH ]; then' build.sh
grep -Fq 'if(!autostart_run())' src/user_shell.c
grep -Fq 'AUTOSTART.SH: running' src/user_shell.c
echo 'check96: PASS'
