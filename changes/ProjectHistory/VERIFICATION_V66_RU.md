# Проверка Toy OS v66 — UDP телеметрия

## Статическая проверка

`./check103.sh` должен закончиться `PASS`.

## Сборка

В W64DevKit x86 выполнить обычные `build.sh` или `build.bat`. Все прежние проверки v65.15 должны пройти.

## QEMU/TAP

Для текущего драйвера использовать проверенную связку:

```bat
qemu-system-i386.exe ^
  -drive format=raw,file=build/disk.img ^
  -netdev tap,id=net0,ifname=tap0,script=no,downscript=no ^
  -device ne2k_isa,netdev=net0
```

## Master/Slave

Master: локальный `NET.CFG` -> `IP=192.168.100.10`; выполнить `exec NETDRV.EXE TELEMETRY MASTER 5000`.

Slave 1: `IP=192.168.100.11`; выполнить `exec NETDRV.EXE TELEMETRY SLAVE1 192.168.100.10`.

Slave 2: `IP=192.168.100.12`; выполнить `exec NETDRV.EXE TELEMETRY SLAVE2 192.168.100.10`.

Ожидается одна строка на каждый slave, обновляемая примерно один раз в секунду; значение COUNT растёт, TICK меняется. При отсутствии пакетов более примерно 3 секунд slot отмечается `OFFLINE`.

## Локальный тест без трёх машин

До межкомпьютерной проверки выполнить в Toy OS: `udp-test`. Команда должна вывести `udp-test: PASS`, а текст в строке 20 появиться без прокрутки экрана.
