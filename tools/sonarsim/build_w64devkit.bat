@echo off
setlocal
cd /d %~dp0
gcc -std=c99 -O2 -Wall -Wextra -Werror sonarsim.c -o SONARSIM.EXE -lws2_32
if errorlevel 1 exit /b 1
SONARSIM.EXE --selftest
if errorlevel 1 exit /b 1
echo FIX59 SONARSIM build/selftest PASS
