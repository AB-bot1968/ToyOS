#!/bin/sh
set -eu
grep -Fq 'static uint32_t execute_line(char*line)' src/user_shell.c
grep -Fq 'static uint32_t autostart_run(void)' src/user_shell.c
grep -Fq 'execute_line(line);' src/user_shell.c
grep -Fq 'sys_file_open(AUTOSTART_NAME,FAT16_MODE_READ)' src/user_shell.c
grep -Fq 'AUTOSTART_FAT_ARG=""' build.sh
grep -Fq 'build/AUTOSTART.SH=AUTOSTART.SH' build.sh
test -f resources/AUTOSTART.SH
grep -q '^exec VGADRV.EXE SPLASH.RAW$' resources/AUTOSTART.SH
echo 'check95: PASS'
