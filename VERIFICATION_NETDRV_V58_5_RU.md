# NETDRV v58.5 — исправление TCP SYN-ACK и проверка системных вызовов

## Найденная причина `no TCP SYN-ACK`

Ошибка была не в ARP и не в ABI системных вызовов.

После отправки SYN драйвер получал корректный входящий TCP SYN-ACK, но функция
`tcp_wait_syn()` проверяла номера портов в неправильном направлении:

- было: `source == client_port` и `destination == server_port`;
- правильно для входящего пакета: `source == server_port` и `destination == client_port`.

Поэтому каждый настоящий SYN-ACK от `10.0.2.2:8080` к временному порту NETDRV
отбрасывался как пакет от другого соединения. Это непосредственно объясняло
последовательность:

```text
NETDRV: ARP OK
NETDRV: no TCP SYN-ACK
NETDRV: TCP connect failed
```

Исправлены все три места входящей TCP-проверки:

1. `tcp_wait_syn()` — SYN-ACK;
2. `tcp_wait_ack()` — ACK/данные от сервера;
3. `recv_file_http()` — HTTP-данные от сервера.

Теперь везде используется направление:

```text
server port (t->dport) -> client port (t->sport)
```

## Проверка системных вызовов

NETDRV использует только существующий ABI:

| Syscall | Назначение |
|---:|---|
| 3 | `SYS_TIMER_GET` |
| 6 | `SYS_FILE_OPEN` |
| 7 | `SYS_FILE_READ` |
| 8 | `SYS_FILE_WRITE` |
| 9 | `SYS_FILE_CLOSE` |
| 12 | `SYS_EXIT` |
| 13 | `SYS_PORT_OUT8` |
| 14 | `SYS_PORT_IN8` |

В `kernel.c` проверено наличие соответствующих `case` и совпадение номеров.
Изменения ядра для NETDRV не требуются.

`SYS_PORT_OUT8/IN8` действительно выполняют физический `outb/inb` в Ring-0,
а NETDRV обращается к ним через `INT 80h`. Это соответствует архитектуре проекта.

## Дополнительные исправления, сохранённые из предыдущих версий

- IPv4 `Total Length` формируется как полный размер IP-пакета, без повторного
  добавления IP-заголовка.
- RX NE2000 не переводится в `STOP` во время обычного polling.
- RX ring начинается с `BNRY=PSTART-1`.
- Remote DMA ожидает `ISR.RDC`.
- Ethernet frame дополняется до минимальных 60 байт.
- `NET.CFG` автоматически копируется из `src/NET.CFG` в build и содержит
  `PORT=8080`.
- Для QEMU включён `RCR=0x14` (`PRO+AB`), что дополнительно исключает зависимость
  приёма от MAC-фильтра.

## Локальная проверка исходника

В среде проверки выполнены три 32-битные компиляции `src/netdrv.c`:

```text
PASS1
PASS2
PASS3
SYSCALL_ABI_CHECK_OK
TCP_PORT_DIRECTION_CHECK_OK
```

Проверки выполнены через `gcc -m32`.

Полную `build.sh` с W64DevKit в этой среде выполнить невозможно: здесь native
compiler имеет target `x86_64-linux-gnu`, а проект намеренно требует i686 W64DevKit.
Также здесь нет QEMU, поэтому реальный запуск Toy OS/QEMU в этой среде не
проводился.

## Запуск на Windows 7

Сначала пересобрать новый образ, чтобы в FAT16 попал новый `NETDRV.EXE`:

```sh
rm -rf build
./build.sh
```

Python 3:

```bat
call python -m http.server 8080
```

QEMU:

```bat
qemu-system-i386 -hda build/toy_os.img -m 16M -netdev user,id=n0 -device ne2k_isa,netdev=n0,mac=52:54:00:12:34:56
```

Toy OS:

```text
cat NET.CFG
exec NETDRV.EXE 10.0.2.2 RECV PAGE.TXT
```

Ожидается:

```text
NETDRV: NE2000 init
NETDRV: ARP 10.0.2.2
NETDRV: ARP OK
NETDRV: TCP connect 10.0.2.2
NETDRV: RECV page saved
NETDRV: OK
```

QEMU документирует стандартную user-mode сеть как `10.0.2.0/24`, с guest
`10.0.2.15` и guest-visible host `10.0.2.2`. citeturn0search0turn0search1

QEMU NE2000 также подтверждает обработку unicast/broadcast через RCR и структуру
RX ring; это использовано при проверке драйвера. citeturn2view0turn3view1
