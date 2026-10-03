# План тестирования Toy OS v66.6

## 1. Сборка

W64DevKit x86:

    ./build.sh

Ожидается создание `build/disk.img` без ошибок.

## 2. Проверка наличия NETDRV.EXE

Сборка сама проверяет корневой `NETDRV.EXE` и `BIN/NETDRV.EXE` через FAT16 checker.

## 3. Запуск QEMU

    qemu-system-i386.exe ^
      -drive format=raw,file=build\\disk.img ^
      -netdev tap,id=net0,ifname=tap0,script=no,downscript=no ^
      -device ne2k_isa,netdev=net0

## 4. Master

В Toy OS:

    exec NETDRV.EXE TELEMETRY MASTER 5000

Ожидается:

    NETDRV: NE2000 init
    NETDRV: UDP MASTER listening

Неправильным результатом считается сообщение `exec NETDRV.EXE: FAIL -3`.

## 5. Тест без Slave

Оставить Master работающим минимум 30 секунд.
Он не должен завершиться самопроизвольно.

## 6. Остановка

Нажать `ESC`.
Ожидается возврат в `toy0>`.

## 7. Slave1

На втором ПК:

    exec NETDRV.EXE TELEMETRY SLAVE1 10.66.1.2

Slave должен продолжать работать при недоступном Master и затем переходить к передаче после появления связи.
