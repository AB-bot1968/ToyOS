@echo off
setlocal EnableExtensions
set "ROOT=%~dp0"
set "REPO=%~1"
if "%REPO%"=="" set "REPO=https://github.com/AB-bot1968/ToyOS.git"
set "WORK=%TEMP%\toyos_fix60zei_publish_%RANDOM%_%RANDOM%"
set "TAG=v67.11-B-FIX60ZEI"

chcp 65001 >nul 2>&1
pushd "%ROOT%" || exit /b 1
call git_preflight.bat || (popd & exit /b 1)
where git >nul 2>nul || (echo ERROR: git not found & popd & exit /b 1)
where powershell.exe >nul 2>nul || (echo ERROR: powershell.exe not found & popd & exit /b 1)

git clone "%REPO%" "%WORK%" || (popd & exit /b 1)

rem Copy exact publication tree but preserve the clone's .git directory.
robocopy "%ROOT%" "%WORK%" /E /R:1 /W:1 /XD .git build dist >nul
if errorlevel 8 (echo ERROR: robocopy failed & rmdir /S /Q "%WORK%" & popd & exit /b 1)

pushd "%WORK%" || (rmdir /S /Q "%WORK%" & popd & exit /b 1)
powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -File "%WORK%\set_git_identity_windows.ps1"
if errorlevel 1 (popd & rmdir /S /Q "%WORK%" & popd & exit /b 1)
call git_preflight.bat || (popd & rmdir /S /Q "%WORK%" & popd & exit /b 1)

git add -A
git diff --cached --quiet
if errorlevel 1 git commit -m "Publish ToyOS v67.11-B FIX60ZEI stable baseline" || (popd & rmdir /S /Q "%WORK%" & popd & exit /b 1)

git rev-parse "%TAG%" >nul 2>nul
if errorlevel 1 git tag -a "%TAG%" -m "ToyOS v67.11-B FIX60ZEI stable baseline" || (popd & rmdir /S /Q "%WORK%" & popd & exit /b 1)

git push origin HEAD:main || (popd & rmdir /S /Q "%WORK%" & popd & exit /b 1)
git push origin "refs/tags/%TAG%" || (popd & rmdir /S /Q "%WORK%" & popd & exit /b 1)

echo PUBLISHED: %REPO%
git rev-parse HEAD
popd
rmdir /S /Q "%WORK%"
popd
endlocal
