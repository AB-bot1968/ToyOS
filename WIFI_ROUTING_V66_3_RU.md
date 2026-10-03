# Toy OS v66.3 — Wi-Fi без L2 Bridge

## Назначение

v66.3 использует обычную IP-маршрутизацию Windows 7 между TAP и Wi-Fi. Windows Network Bridge не создаётся.

На ПК №1 работает Master:

- Windows Wi-Fi: реальный адрес ПК №1 в домашней/локальной сети, например `192.168.1.10`.
- `tap0` Windows: `10.66.1.1/24`.
- Toy OS Master в QEMU: `10.66.1.2/24`, gateway `10.66.1.1`.

На ПК №2 работает Slave:

- Windows Wi-Fi: реальный адрес ПК №2, например `192.168.1.20`.
- `tap0` Windows: `10.66.2.1/24`.
- Toy OS Slave1: `10.66.2.2/24`, gateway `10.66.2.1`.
- Toy OS Slave2 при необходимости: `10.66.2.3/24`.

Реальные Wi-Fi адреса `192.168.1.10` и `192.168.1.20` приведены только как пример. Используйте фактические адреса из `ipconfig`.

## Важно

`tap0` не получает IP Toy OS. IP на `tap0` принадлежит Windows и служит шлюзом между QEMU и Windows TCP/IP stack.

В QEMU сохраняется уже проверенная схема:

`-netdev tap,id=net0,ifname=tap0,script=no,downscript=no`

и:

`-device ne2k_isa,netdev=net0`

## 1. Узнать Wi-Fi IPv4

На каждом ПК открыть cmd и выполнить:

```bat
ipconfig
```

Запишите IPv4-адрес Wi-Fi:

- ПК №1 = `PC1_WIFI_IP`
- ПК №2 = `PC2_WIFI_IP`

Оба ПК должны находиться в одной обычной Wi-Fi сети и видеть друг друга по Windows ping.

## 2. Назначить адрес tap0 на ПК №1

Откройте cmd от имени администратора.

```bat
netsh interface ipv4 set address name="tap0" static 10.66.1.1 255.255.255.0
```

Проверка:

```bat
ipconfig
```

У `tap0` должен быть `10.66.1.1`.

## 3. Назначить адрес tap0 на ПК №2

От имени администратора:

```bat
netsh interface ipv4 set address name="tap0" static 10.66.2.1 255.255.255.0
```

Проверка:

```bat
ipconfig
```

## 4. Включить IP routing в Windows 7

Откройте cmd от имени администратора и выполните:

```bat
reg add "HKLM\SYSTEM\CurrentControlSet\Services\Tcpip\Parameters" /v IPEnableRouter /t REG_DWORD /d 1 /f
```

После изменения рекомендуется перезагрузка. Microsoft Q&A для Windows 7 описывает этот параметр `IPEnableRouter=1` как способ включения IP-маршрутизации; там же упоминается служба Routing and Remote Access. citeturn175197search0turn175197search3

Проверка после перезагрузки:

```bat
ipconfig /all
```

В нормальной конфигурации Windows показывает, что IP routing enabled.

## 5. Добавить статический маршрут на ПК №1

`PC2_WIFI_IP` замените реальным Wi-Fi адресом ПК №2.

От имени администратора:

```bat
route -p add 10.66.2.0 mask 255.255.255.0 PC2_WIFI_IP
```

Пример, если ПК №2 имеет Wi-Fi `192.168.1.20`:

```bat
route -p add 10.66.2.0 mask 255.255.255.0 192.168.1.20
```

Проверка:

```bat
route print 10.66.2.0
```

Команда `route /p add` создаёт постоянный маршрут; синтаксис подтверждён документацией Microsoft. citeturn175197search1turn175197search4

## 6. Добавить обратный маршрут на ПК №2

`PC1_WIFI_IP` замените реальным Wi-Fi адресом ПК №1.

```bat
route -p add 10.66.1.0 mask 255.255.255.0 PC1_WIFI_IP
```

Пример:

```bat
route -p add 10.66.1.0 mask 255.255.255.0 192.168.1.10
```

Проверка:

```bat
route print 10.66.1.0
```

## 7. Базовая проверка Windows

На обоих ПК:

```bat
ping localhost
```

Затем с ПК №1:

```bat
ping PC2_WIFI_IP
```

С ПК №2:

```bat
ping PC1_WIFI_IP
```

Эти проверки должны проходить до запуска QEMU.

После запуска QEMU на ПК №1 можно проверить локальную сторону:

```bat
ping 10.66.1.1
```

На ПК №2:

```bat
ping 10.66.2.1
```

## 8. Запуск QEMU Master

На ПК №1:

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

Ожидается:

```text
NETDRV: NE2000 init
NETDRV: UDP MASTER listening
```

## 9. Запуск Slave1

На ПК №2:

```bat
qemu-system-i386.exe ^
  -drive format=raw,file=build\disk.img ^
  -netdev tap,id=net0,ifname=tap0,script=no,downscript=no ^
  -device ne2k_isa,netdev=net0
```

В Toy OS:

```text
exec NETDRV.EXE TELEMETRY SLAVE1 10.66.1.2
```

Теперь Slave должен определить, что `10.66.1.2` находится вне своей `10.66.2.0/24`, и поэтому ARP должен идти к Windows TAP-шлюзу `10.66.2.1`:

```text
NETDRV: route ARP 10.66.2.1
NETDRV: ARP OK
```

После этого Slave начинает отправлять UDP на реальный IP Toy OS Master `10.66.1.2`; Windows доставляет пакет по маршруту через Wi-Fi.

## 10. Проверка Master

На экране Master должна обновляться область:

```text
SLAVE1 | SLAVE1 COUNT=...
```

Старая строка не должна накапливаться.

## 11. Проверка отключения Master

Остановите Master клавишей:

`ESC`

Slave не должен завершаться. Он продолжает работать и повторять ARP к `10.66.2.1`/пытаться отправлять после восстановления маршрута.

## 12. Проверка восстановления

Сначала запустите Slave, когда Master выключен. Ожидается, что Slave останется в работе.

Затем запустите Master. Через следующий цикл ARP Slave должен получить `ARP OK` и возобновить телеметрию.

## 13. Вторая роль без третьего ПК

На ПК №2 можно отдельно проверить:

```text
exec NETDRV.EXE TELEMETRY SLAVE2 10.66.1.2
```

Его IP внутри Toy OS — `10.66.2.3`.

Одновременно запускать SLAVE1 и SLAVE2 на одном QEMU мы на этом этапе не требуем: оба используют один и тот же `tap0` и одну Windows TAP-сеть, а тестируем роли последовательно.

## 14. Если `ARP 10.66.2.1` остаётся FAILED

Проверять в таком порядке:

1. `ipconfig` — у tap0 должен быть `10.66.2.1`.
2. QEMU действительно использует `ifname=tap0`.
3. На ПК №2 `ping 10.66.2.1` выполняется до запуска Toy OS.
4. `route print` содержит обратный маршрут.
5. Windows IP routing включён.
6. В контролируемой домашней сети временно проверить влияние Windows Firewall. Не оставляйте защиту отключённой после теста.

## 15. Почему это работает

Slave1 имеет:

`10.66.2.2/24` → шлюз `10.66.2.1`.

Master имеет:

`10.66.1.2/24` → шлюз `10.66.1.1`.

Удалённый Master находится за пределами локальной подсети Slave1, поэтому Slave ARP-ит Windows на `10.66.2.1`. Windows получает Ethernet-кадр от TAP и маршрутизирует IPv4 дальше через Wi-Fi. Обратный маршрут на ПК №2 позволяет ответному трафику попасть обратно в виртуальную подсеть Master.
