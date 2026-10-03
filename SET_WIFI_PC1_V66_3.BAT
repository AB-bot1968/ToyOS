@echo off
setlocal
rem Toy OS v66.3 — ПК №1 (MASTER).
rem Задайте здесь реальный Wi-Fi IP ПК №2 перед запуском.
set PC2_WIFI_IP=192.168.1.20

echo [1/3] Configure TAP tap0 = 10.66.1.1/24
netsh interface ipv4 set address name="tap0" static 10.66.1.1 255.255.255.0
if errorlevel 1 goto :err

echo [2/3] Enable IPv4 routing in Windows
reg add "HKLM\SYSTEM\CurrentControlSet\Services\Tcpip\Parameters" /v IPEnableRouter /t REG_DWORD /d 1 /f
if errorlevel 1 goto :err

echo [3/3] Add route to PC2 virtual network via PC2 Wi-Fi
route -p add 10.66.2.0 mask 255.255.255.0 %PC2_WIFI_IP%
if errorlevel 1 goto :err

echo.
echo PC1 setup completed. Reboot Windows if IP routing was just enabled.
goto :eof
:err
echo ERROR: run this BAT as Administrator.
exit /b 1
