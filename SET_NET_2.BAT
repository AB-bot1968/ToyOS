@echo off
copy /Y NET_SLV2.CFG src\NET.CFG >nul
if errorlevel 1 echo ERROR: cannot install SLAVE2 NET.CFG&exit /b 1
 echo SLAVE2 NET.CFG installed: 192.168.100.12
