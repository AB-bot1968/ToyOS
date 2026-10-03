@echo off
setlocal EnableExtensions
cd /d "%~dp0"
where powershell.exe >nul 2>&1
if errorlevel 1 (
  echo ERROR: powershell.exe was not found. Windows PowerShell is required for native SHA-256 preflight.
  exit /b 1
)
chcp 65001 >nul 2>&1
powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -File "%~dp0git_preflight_windows.ps1"
exit /b %ERRORLEVEL%
