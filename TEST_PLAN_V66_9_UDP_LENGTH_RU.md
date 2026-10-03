# План проверки Toy OS v66.9 — исправление длины UDP-кадра

## Цель

Проверить, что Slave отправляет полный Ethernet-кадр IPv4/UDP, включая 14-байтный Ethernet-заголовок, и что Master принимает телеметрию.

## 1. Статическая проверка

В W64DevKit x86:

    ./check122.sh

Ожидается:

    check122: PASS

## 2. Сборка

Выполнить:

    build.bat

Ожидается создание `build\\disk.img` без ошибок.

## 3. Запуск Master на ПК1

    qemu-system-i386.exe ^
      -drive format=raw,file=build\\disk.img ^
      -netdev tap,id=net0,ifname=tap0,script=no,downscript=no ^
      -device ne2k_isa,netdev=net0

В Toy OS:

    exec NETDRV.EXE TELEMETRY MASTER 5000

Ожидается:

    NETDRV: NE2000 init
    NETDRV: UDP MASTER listening

## 4. Запуск Slave1 на ПК2

    exec NETDRV.EXE TELEMETRY SLAVE1 10.66.1.2

Ожидается:

    NETDRV: route ARP 10.66.2.1
    NETDRV: ARP OK
    UDP TX 1
    UDP TX 2
    UDP TX 3

## 5. Проверка Master

На Master должны увеличиваться счётчики RX, а строка SLAVE1 должна обновляться на месте.

## 6. Остановка

ESC на Master и Slave должен вернуть `toy0>`.

## 7. Регрессия

Статические тесты предыдущих версий UDP должны оставаться PASS.
