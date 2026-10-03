@echo off
rem PNG2RAW.BAT — запуск собранной GCC-утилиты PNG2RAW.EXE для Windows 7.
rem Сам EXE создается автоматически скриптом build.sh/build.bat.

setlocal
set "TOOL=%~dp0PNG2RAW.EXE"

if "%~1"=="" (
    echo Usage: PNG2RAW.BAT input.png [output.raw]
    echo.
    echo Example: PNG2RAW.BAT my_splash.png resources\SPLASH.RAW
    exit /b 2
)

if not exist "%TOOL%" (
    echo PNG2RAW: ERROR: tools\PNG2RAW.EXE not found.
    echo First run build.bat in the W64DevKit x86 shell to compile the utility.
    exit /b 1
)

if "%~2"=="" (
    "%TOOL%" "%~1"
) else (
    "%TOOL%" "%~1" "%~2"
)

set "RC=%ERRORLEVEL%"
endlocal & exit /b %RC%
