@echo off
setlocal
cd /d %~dp0
where sh >nul 2>&1
if errorlevel 1 (
  echo ERROR: Run this script from the W64DevKit shell.
  exit /b 1
)
sh -lc "./build.sh"
if errorlevel 1 exit /b 1
echo Build and verification OK.
endlocal
