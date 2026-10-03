@echo off
setlocal
cd /d %~dp0
sh -lc "./check17.sh"
if errorlevel 1 exit /b 1
echo syscall-test checks OK.
endlocal
