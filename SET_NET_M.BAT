@echo off
copy /Y NET_MAST.CFG src\NET.CFG >nul
if errorlevel 1 echo ERROR: cannot install MASTER NET.CFG&exit /b 1
 echo MASTER NET.CFG installed: 192.168.100.10
