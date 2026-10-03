@echo off
setlocal
cd /d %~dp0
where sh >nul 2>&1
if errorlevel 1 (echo ERROR: Run from W64DevKit shell.& exit /b 1)
sh -lc "./check10.sh"
if errorlevel 1 exit /b 1
endlocal
