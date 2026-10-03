# Подробный план тестирования Toy OS v66.7

## 1. Сборка

В W64DevKit x86 на Windows 7:

    ./check118.sh
    ./build.sh

Ожидается `check118: PASS` и файл `build/disk.img`.

## 2. Запуск QEMU

    qemu-system-i386.exe ^
      -drive format=raw,file=build\\disk.img ^
      -netdev tap,id=net0,ifname=tap0,script=no,downscript=no ^
      -device ne2k_isa,netdev=net0

## 3. Slave без Master

    exec NETDRV.EXE TELEMETRY SLAVE1 10.66.1.2

Ожидается:

    NETDRV: NE2000 init
    NETDRV: route ARP 10.66.2.1
    NETDRV: press ESC to stop slave
    NETDRV: ARP WAIT 10.66.2.1

Slave должен оставаться запущенным. После каждой неудачной попытки допустимо новое `ARP WAIT`.

## 4. Проверка остановки Slave

Нажать `ESC`.

Ожидается:

    NETDRV: UDP SLAVE stopped
    toy0>

## 5. Slave с доступным TAP-шлюзом

Снова выполнить команду Slave. После доступности Windows TAP:

    NETDRV: ARP OK
    NETDRV: UDP TX 1
    NETDRV: UDP TX 2
    NETDRV: UDP TX 3

Счётчик должен возрастать примерно раз в секунду.

## 6. Проверка Master

На ПК №1 запустить:

    exec NETDRV.EXE TELEMETRY MASTER 5000

После передачи Slave на Master должна изменяться строка `SLAVE1`, а счётчик RX должен возрастать.

## 7. Отсутствие накопления

На Slave состояние и TX-счётчик всегда отображаются в фиксированной строке. Старые строки не должны добавляться вниз.

## 8. Проверка потери Master

Остановить Master. Slave не должен завершаться. Он должен перейти в повторный `ARP WAIT`, а после появления Master снова получить `ARP OK` и продолжить `UDP TX`.
