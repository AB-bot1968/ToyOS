@echo off
copy /Y NET_SLV1.CFG src\NET.CFG >nul
if errorlevel 1 echo ERROR: cannot install SLAVE1 NET.CFG&exit /b 1
 echo SLAVE1 NET.CFG installed: 192.168.100.11
