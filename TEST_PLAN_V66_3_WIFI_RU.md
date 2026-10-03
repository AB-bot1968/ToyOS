# Toy OS v66.3 — подробное тестирование UDP-телеметрии через Wi-Fi

## Цель

Проверить работу двух компьютеров Windows 7 без Windows Network Bridge:

`Toy OS Master → QEMU → tap0 → Windows 7 → Wi-Fi → Windows 7 → tap0 → QEMU → Toy OS Slave1`

Пакеты являются IPv4/UDP. Каждый Windows-хост маршрутизирует между своим TAP и Wi-Fi.

## Схема адресов

ПК №1 / Master:

- Wi-Fi IPv4: `PC1_WIFI_IP` — фактический адрес из `ipconfig`.
- TAP `tap0`: `10.66.1.1/24`.
- Toy OS Master: `10.66.1.2/24`.
- Gateway Master: `10.66.1.1`.

ПК №2 / Slave:

- Wi-Fi IPv4: `PC2_WIFI_IP` — фактический адрес из `ipconfig`.
- TAP `tap0`: `10.66.2.1/24`.
- Toy OS Slave1: `10.66.2.2/24`.
- Toy OS Slave2: `10.66.2.3/24`.
- Gateway Slave: `10.66.2.1`.

Master и Slave находятся в разных виртуальных подсетях. Поэтому Slave ARP-ит только собственный Windows gateway `10.66.2.1`, а не удалённый адрес `10.66.1.2`.

## Шаг 0. Условия

- Два ПК подключены к одной обычной Wi-Fi сети.
- TAP-Windows 9.21.2 установлен и не имеет Code 52.
- В `ncpa.cpl` TAP переименован в `tap0`.
- QEMU 5.1.92/5.2 RC поддерживает `tap` и `ne2k_isa`.
- На обоих ПК используется один и тот же исходный проект v66.3.

Wi-Fi сеть должна разрешать прямой обмен между клиентами. В общественных/гостевых сетях может быть включена client isolation, при которой два ПК не видят друг друга.

## Шаг 1. Определить Wi-Fi IP двух ПК

На ПК №1:

```bat
ipconfig
```

Запишите IPv4 Wi-Fi, например `192.168.1.10`.

На ПК №2:

```bat
ipconfig
```

Запишите IPv4 Wi-Fi, например `192.168.1.20`.

До дальнейших действий проверьте обычный Windows ping:

ПК №1 → ПК №2:

```bat
ping 192.168.1.20
```

ПК №2 → ПК №1:

```bat
ping 192.168.1.10
```

Если эти ping не проходят, Toy OS пока тестировать бессмысленно: проблема находится в Wi-Fi сети/Windows firewall.

## Шаг 2. Настроить TAP на ПК №1

Открыть cmd от имени администратора:

```bat
netsh interface ipv4 set address name="tap0" static 10.66.1.1 255.255.255.0
```

Проверить:

```bat
ipconfig
```

Должно быть:

`tap0 = 10.66.1.1`

## Шаг 3. Настроить TAP на ПК №2

Cmd от имени администратора:

```bat
netsh interface ipv4 set address name="tap0" static 10.66.2.1 255.255.255.0
```

Проверить:

```bat
ipconfig
```

Должно быть:

`tap0 = 10.66.2.1`

## Шаг 4. Включить IP routing на обоих Windows

В cmd от имени администратора на каждом ПК:

```bat
reg add "HKLM\SYSTEM\CurrentControlSet\Services\Tcpip\Parameters" /v IPEnableRouter /t REG_DWORD /d 1 /f
```

Перезагрузите Windows.

Microsoft Q&A для Windows 7 описывает этот параметр `IPEnableRouter=1` для включения маршрутизации. В той же инструкции упоминается Routing and Remote Access. citeturn175197search0turn175197search3

После перезагрузки:

```bat
ipconfig /all
```

Проверьте, что маршрутизация IP включена.

## Шаг 5. Добавить маршрут на ПК №1

Заменить `PC2_WIFI_IP` на фактический Wi-Fi IP второго ПК.

```bat
route -p add 10.66.2.0 mask 255.255.255.0 PC2_WIFI_IP
```

Пример:

```bat
route -p add 10.66.2.0 mask 255.255.255.0 192.168.1.20
```

Проверить:

```bat
route print 10.66.2.0
```

Microsoft указывает, что `/p` сохраняет добавленный маршрут между перезапусками. citeturn175197search1turn175197search4

## Шаг 6. Добавить обратный маршрут на ПК №2

Заменить `PC1_WIFI_IP` на фактический Wi-Fi IP первого ПК.

```bat
route -p add 10.66.1.0 mask 255.255.255.0 PC1_WIFI_IP
```

Пример:

```bat
route -p add 10.66.1.0 mask 255.255.255.0 192.168.1.10
```

Проверить:

```bat
route print 10.66.1.0
```

## Шаг 7. Проверить Windows-маршрутизацию до запуска Toy OS

С обоих ПК:

```bat
route print
```

Должен быть маршрут к удалённой виртуальной подсети через Wi-Fi IP второго ПК.

Также убедитесь, что `tap0` и Wi-Fi не получили одинаковую виртуальную подсеть. Здесь намеренно используются разные сети `10.66.1.0/24` и `10.66.2.0/24`.

## Шаг 8. Собрать v66.3

В W64DevKit x86:

```bash
./check109.sh
./check110.sh
./check111.sh
./build.sh
```

Ожидается успешное создание:

`build/disk.img`

Полная сборка должна выполняться именно x86 W64DevKit, потому что build.sh проверяет `gcc -dumpmachine` и требует i686.

## Шаг 9. Запустить Master на ПК №1

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

`listening` — нормальное состояние: Master ждёт UDP.

## Шаг 10. Запустить Slave1 на ПК №2

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

Ключевая строка v66.3:

```text
NETDRV: route ARP 10.66.2.1
```

а НЕ:

```text
NETDRV: route ARP 10.66.1.2
```

После ARP шлюза:

```text
NETDRV: ARP OK
```

Slave продолжает работу независимо от состояния Master.

## Шаг 11. Проверить получение UDP на Master

На Master строка `SLAVE1` должна обновляться на одном и том же месте экрана:

```text
SLAVE1 | SLAVE1 COUNT=...
```

Счётчик должен расти примерно раз в секунду.

Старые значения не должны накапливаться в новых строках.

## Шаг 12. Проверить независимость Slave

Остановите Master клавишей `ESC`.

На Slave1 процесс должен продолжить работу. Он не должен завершаться только потому, что Master выключен.

Снова запустите Master. После восстановления маршрута Slave должен возобновить отправку.

## Шаг 13. Проверить SLAVE2 без третьего компьютера

На ПК №2 остановите Slave1 и выполните:

```text
exec NETDRV.EXE TELEMETRY SLAVE2 10.66.1.2
```

Ожидаемый IP Slave2:

`10.66.2.3`

Ожидаемый локальный ARP target:

`10.66.2.1`

Третья машина не нужна. SLAVE1 и SLAVE2 проверяются последовательно на одном втором ПК.

## Шаг 14. Тест запуска Slave раньше Master

1. Остановить Master.
2. Запустить Slave1.
3. Убедиться, что Slave1 остаётся работать.
4. Подождать несколько секунд.
5. Запустить Master.
6. Дождаться `ARP OK`.
7. Убедиться, что Master получает счётчик.

Это основной тест независимости.

## Шаг 15. Тест потери Wi-Fi

При работающем Master/Slave временно отключить Wi-Fi на ПК №2.

Slave не должен падать с возвратом в shell из-за потери сети.

После восстановления Wi-Fi/маршрута Slave должен заново получить ARP и продолжить передачу.

## Шаг 16. Если `ARP 10.66.2.1` = FAILED

Проверять по порядку:

```bat
ipconfig
```

`tap0` должен быть `10.66.2.1`.

Затем:

```bat
ping 10.66.2.1
```

на ПК №2. Этот адрес принадлежит Windows TAP, поэтому он должен отвечать.

Затем:

```bat
route print
```

и проверить обратный маршрут `10.66.1.0/24` на ПК №2.

Затем проверить:

```bat
ping PC1_WIFI_IP
```

и:

```bat
ping PC2_WIFI_IP
```

Потом проверить, что IP routing включён после перезагрузки.

В крайнем случае временно проверить влияние Windows Firewall в контролируемой домашней сети. После диагностики защиту обязательно вернуть.

## Шаг 17. Критерии успешного v66.3

Тест считается успешным, когда одновременно выполнены условия:

1. TAP `tap0` работает на обоих ПК.
2. QEMU без ошибки открывает TAP.
3. Master пишет `UDP MASTER listening`.
4. Slave пишет `route ARP 10.66.2.1` и `ARP OK`.
5. Slave1 продолжает работать при выключенном Master.
6. После запуска Master начинает расти `COUNT`.
7. ESC возвращает Master в `toy0>`.
8. SLAVE2 можно проверить на том же втором ПК без третьего ПК.
