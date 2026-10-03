#!/bin/sh
set -eu
fail(){ echo "FIX60A EXECUTABLE LOADER CHECK: FAIL: $1"; exit 1; }
grep -q 'static int fat_open_executable' src/kernel.c || fail helper
grep -q "name\[i\]=='/'" src/kernel.c || fail simple-name-guard
grep -q 'fd=fat_open_executable(name);' src/kernel.c || fail loader-use
grep -q 'RUN cd /DOC' TST/TESTEXE.TST || fail cwd-regression
grep -q 'RUN execmt HBEATOK.EXE' TST/TESTEXE.TST || fail real-execmt
grep -q 'ASSERT FILE_EXISTS /SONARDRV.EXE' TST/TESTEXE.TST || fail sonar-presence
grep -q 'FILE_EXISTS ' src/user_shell.c || fail file-exists-assert
grep -q 'TST/TESTEXE.TST' build.sh || fail pack-test
grep -q 'SYS_PORT_OUT8.*13u' src/kernel.c || fail port13
grep -q 'SYS_PORT_IN8.*14u' src/kernel.c || fail port14
echo 'FIX60A EXECUTABLE LOADER CHECK: PASS'
