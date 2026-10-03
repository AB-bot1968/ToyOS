# План тестирования Toy OS v66.11

Цель: проверить только новую консольную раскладку, сохранив доказанную в v66.10 передачу UDP.

## 1. Сборка

На Windows 7 в W64DevKit x86:

```bat
build.bat
```

Ожидается создание `build\disk.img` без ошибок.

## 2. Master

Запустить QEMU с TAP и NE2000:

```bat
qemu-system-i386.exe ^
  -drive format=raw,file=build\disk.img ^
  -netdev tap,id=net0,ifname=tap0,script=no,downscript=no ^
  -device ne2k_isa,netdev=net0
```

В Toy OS:

```text
exec NETDRV.EXE TELEMETRY MASTER 5000
```

Проверить:

1. Экран не должен прокручиваться служебными строками.
2. Должен появиться единый dashboard Master.
3. Блоки SLAVE1 и SLAVE2 должны быть расположены отдельно.
4. До появления пакетов состояние должно быть `WAITING`.
5. Нажатие ESC должно вернуть `toy0>`.

## 3. Slave1

На втором ПК:

```text
exec NETDRV.EXE TELEMETRY SLAVE1 10.66.1.2
```

Проверить:

1. Dashboard Slave не прокручивается.
2. При отсутствии Master состояние ARP остаётся `WAITING`.
3. После доступности Master появляется `READY`/успешный ARP.
4. Счётчик `TRANSMIT` увеличивается примерно раз в секунду.
5. Поле `LAST TELEMETRY` заменяется на месте.
6. ESC возвращает `toy0>`.

## 4. Передача

При одновременно работающих Master и Slave1 проверить на Master:

```text
SLAVE1  STATUS=ONLINE
RX=...
DATA: SLAVE1 COUNT=...
```

Значение DATA должно обновляться на месте, без накопления строк.

## 5. Потеря связи

Остановить Slave1. Через несколько секунд Master должен показать `WAITING` для SLAVE1, не изменяя блок SLAVE2.

## 6. Регрессия

Прогнать `check109.sh` ... `check127.sh`. При наличии i686 GCC все тесты должны дать `PASS`; без i686 GCC тесты компиляции могут корректно завершиться как `SKIP`.
