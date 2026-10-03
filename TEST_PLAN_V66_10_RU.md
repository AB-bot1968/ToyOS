# Тестирование Toy OS v66.10

1. Собрать проект в Windows 7 + W64DevKit x86 через build.bat.
2. Запустить QEMU через TAP:
   qemu-system-i386.exe -drive format=raw,file=build\\disk.img -netdev tap,id=net0,ifname=tap0,script=no,downscript=no -device ne2k_isa,netdev=net0
3. На ПК1 выполнить: exec NETDRV.EXE TELEMETRY MASTER 5000.
4. На ПК2 выполнить: exec NETDRV.EXE TELEMETRY SLAVE1 10.66.1.2.
5. На Slave проверить ARP OK и UDP TX 1, UDP TX 2, ...
6. На Master должен появиться NETDRV: ARP reply 10.66.1.2, затем NETDRV: UDP RX SLAVE1 count=1, count=2, ...
7. В поле SLAVE1 должна постоянно заменяться текущая строка телеметрии. Новые строки не должны скапливаться.
8. Остановить Master клавишей ESC. Slave не должен завершаться из-за отсутствия Master и должен продолжать повторный ARP/передачу.
9. Повторить с SLAVE2, используя второй ПК без третьей машины.
