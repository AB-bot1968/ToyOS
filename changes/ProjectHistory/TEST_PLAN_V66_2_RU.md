# Toy OS v66.2 — подробный план проверки

## 1. Сборка

В W64DevKit x86:

`./build.sh`

Ожидается файл `build/disk.img` без ошибок сборки.

## 2. Статические тесты

`./check103.sh`
`./check104.sh`
`./check105.sh`
`./check106.sh`
`./check107.sh`
`./check108.sh`

Все должны завершиться `PASS`.

## 3. Тест Slave без Master

Запусти QEMU с TAP и NE2000.

`exec NETDRV.EXE TELEMETRY SLAVE1 192.168.100.10`

Ожидается `UDP SLAVE running independently`. Slave не должен завершаться через 10 секунд с `ARP failed`.

Оставь его работать. Запусти Master позже. После появления L2-связи Slave должен напечатать `ARP OK` и начать передачу.

## 4. Тест Master ESC

На Master:

`exec NETDRV.EXE TELEMETRY MASTER 5000`

Нажми `ESC`.

Ожидается:

`NETDRV: UDP MASTER stopped`

и возврат к `toy0>`.

## 5. Межмашинный ARP

Для двух ПК сначала создать Windows Network Bridge: на каждом ПК объединить `tap0` с реальным LAN-адаптером через `ncpa.cpl`.

Master: `192.168.100.10/24`
Slave1: `192.168.100.11/24`

На Slave1:

`exec NETDRV.EXE TELEMETRY SLAVE1 192.168.100.10`

Ожидается `ARP OK`.

## 6. UDP Master + Slave1

Master продолжает `listening`.
Slave1 отправляет пакет раз в секунду.

Master отображает данные в строке SLAVE1, перезаписывая прежнее значение.

## 7. Отказ Slave1

Остановить Slave1. Master через несколько секунд должен показать `SLAVE1 OFFLINE`, но продолжить работать.

## 8. Независимость Slave2 без третьего ПК

На втором ПК вместо SLAVE1 выполнить:

`exec NETDRV.EXE TELEMETRY SLAVE2 192.168.100.10`

Ожидается самостоятельный ARP и передача с `192.168.100.12`. SLAVE1 при этом не нужен.
