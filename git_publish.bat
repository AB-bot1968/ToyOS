@echo off
setlocal EnableExtensions
cd /d "%~dp0"
where sh >nul 2>&1
if errorlevel 1 (
  echo ERROR: sh.exe was not found. Run from W64DevKit shell or Git Bash.
  exit /b 1
)
sh ./git_publish.sh %*
exit /b %ERRORLEVEL%
