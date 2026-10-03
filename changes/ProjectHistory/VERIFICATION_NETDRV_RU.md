# Toy OS v58 — NETDRV.EXE

## Назначение

`NETDRV.EXE` — самостоятельная Ring-3 программа EXE1. Ядро не содержит TCP/IP
стек и не содержит отдельного сетевого драйвера. Программа использует уже
существующие syscall 13/14 для безопасного доступа к I/O-портам.

Аппаратная модель теста — **QEMU NE2000 ISA**. Это выбрано специально: NE2000
имеет собственную packet RAM и позволяет реализовать обмен из Ring-3 через
remote DMA PIO без добавления DMA-буферов или новых системных вызовов.

Поддерживается:

- Ethernet;
- ARP;
- IPv4;
- минимальный TCP client;
- HTTP/1.0 GET;
- HTTP/1.0 POST;
- сохранение HTTP body в FAT16-файл.

Не поддерживаются DNS, IPv6, VLAN, chunked HTTP, HTTPS/TLS и полноценная
реализация TCP retransmission/window scaling.

## Команда

```text
exec NETDRV.EXE IP SEND FILE.TXT
exec NETDRV.EXE IP RECV FILE.TXT
```

Shell передаёт три аргумента через уже существующий `SYS_EXEC_ARGS`:

```text
EBX = IP
ECX = SEND/RECV
EDX = FILE.TXT
```

Изменение ядра не потребовалось.

## NET.CFG

Файл имеет фиксированное имя `NET.CFG`.

Пример для стандартной QEMU user-mode сети:

```text
BASE=300
MAC=52:54:00:12:34:56
IP=10.0.2.15
MASK=255.255.255.0
GATEWAY=10.0.2.2
PORT=80
PATH=/
TIMEOUT=10
```

`IP` в конфигурации — адрес самой Toy OS. IP назначения берётся из команды.

Если адрес назначения находится в локальной подсети, ARP выполняется для него.
Иначе ARP выполняется для `GATEWAY`.

## QEMU

Современный QEMU:

```text
qemu-system-i386 -hda build/toy_os.img -m 16M -netdev user,id=n0 -device ne2k_isa,netdev=n0,mac=52:54:00:12:34:56
```

Старый QEMU:

```text
qemu-system-i386 -hda build/toy_os.img -m 16M -net user -net nic,model=ne2k_isa,macaddr=52:54:00:12:34:56
```

QEMU поддерживает `ne2k_isa`, а user-mode networking по умолчанию использует
сеть `10.0.2.0/24` и адрес хоста `10.0.2.2`. citeturn1search0turn1search12

## Проверка RECV

На Windows 7 на основной машине можно поднять HTTP-сервер на порту 80/8080.
Для теста доступа к хосту из QEMU user-mode используется адрес `10.0.2.2`.

Например, если сервер слушает порт 8080:

```text
PORT=8080
PATH=/page.txt
```

и файл доступен сервером как `/page.txt`, команда:

```text
exec NETDRV.EXE 10.0.2.2 RECV PAGE.TXT
```

должна создать `PAGE.TXT` с телом HTTP-ответа.

## Проверка SEND

Для `SEND` удалённая сторона должна принимать HTTP POST:

```text
POST /upload HTTP/1.0
Content-Length: N
Content-Type: application/octet-stream
```

В `NET.CFG`:

```text
PORT=8080
PATH=/upload
```

Команда:

```text
exec NETDRV.EXE 10.0.2.2 SEND TEST.TXT
```

передаёт содержимое `TEST.TXT` телом POST.

## Почему выбрана NE2000 ISA

RTL8139/e1000 потребовали бы DMA-буферов, доступных ядру, либо нового kernel
syscall для передачи/приёма Ethernet-кадров. Для требования «драйвер является
EXE1» это добавило бы существенно больше изменений в ядро. NE2000 позволяет
сохранить границу Ring-3 и использовать уже существующие port-I/O syscall.

Регистр remote-DMA и схема ring-buffer соответствуют классической NE2000/8390
модели. citeturn0search0turn0search3

## Проверка компиляции

Добавлен `check59.sh`. Он выполняет три отдельных прохода:

1. `-fsyntax-only`;
2. полноценная компиляция `netdrv.c` в объект;
3. повторная полноценная компиляция с `-fno-omit-frame-pointer`.

После этого проверяются `NETDRV.EXE` и наличие `NET.CFG` в FAT16-образе.

В локальной среде разработки выполнены дополнительные 32-битные проверки:
синтаксис, объектная компиляция и PE/COFF-компиляция/линковка. Фактический
W64DevKit/x86 запуск `build.sh` в текущем окружении не выполнялся, поэтому это
не объявляется как проверка именно W64DevKit.

## Изменения относительно v58

Изменены только:

- `src/netdrv.c` — новый EXE1;
- `src/user_shell.c` — специальная передача трёх аргументов NETDRV;
- `build.sh` — сборка/упаковка NETDRV и включение `NET.CFG`;
- `NET.CFG` — пример фиксированного конфигурационного файла;
- `check59.sh` и эта документация.

`src/kernel.c`, ABI syscall и формат EXE1 не изменялись.
