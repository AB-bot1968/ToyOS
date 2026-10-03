# NETDRV.EXE1 v58 — исправление RX/TX и TCP connect

## Причина

Предыдущая версия при polling выбирала page 1 командой с битом `STOP`, а затем
снова запускала NE2000. Это создавало ненужное окно остановки приёмника между
отправкой TCP SYN и чтением CURR. Для TCP-клиента это особенно плохо: SYN-ACK
может прийти именно в это окно.

Дополнительно передача коротких Ethernet-кадров содержала ошибку: длина кадра
сначала увеличивалась до 60 байт, поэтому последующее заполнение padding никогда
не выполнялось.

## Исправления

1. Для работающего NE2000 используется выбор страницы `START|NODMA|PAGE1`.
2. `STOP|NODMA|PAGE1` используется только во время начальной настройки.
3. RCR установлен в стандартный режим приема Ethernet broadcast (`AB=1`), а
   unicast принимается по настроенному MAC.
4. Padding Ethernet до 60 байт выполняется до запуска remote DMA.
5. IPv4 Total Length остается `20 + 20 + TCP data`, без повторного добавления
   IPv4 header.
6. После трех попыток TCP connect выводится `NETDRV: no TCP SYN-ACK`.

## Компиляция

Проверка рассчитана на 32-битный W64DevKit:

```text
./check_netdrv_v58_final.sh
```

Скрипт выполняет три независимых прохода:

1. `-fsyntax-only`;
2. обычную компиляцию объекта;
3. компиляцию объекта с `-fno-omit-frame-pointer`.

## Runtime-проверка RECV

В `src/NET.CFG`:

```text
BASE=300
MAC=52:54:00:12:34:56
IP=10.0.2.15
MASK=255.255.255.0
GATEWAY=10.0.2.2
PORT=8080
PATH=/
TIMEOUT=10
```

На Windows 7:

```text
call python -m http.server 8080
```

Собрать новый образ:

```text
./build.sh
```

Запустить QEMU:

```text
qemu-system-i386 -hda build/toy_os.img -m 16M -netdev user,id=n0 -device ne2k_isa,netdev=n0,mac=52:54:00:12:34:56
```

В Toy OS:

```text
exec NETDRV.EXE 10.0.2.2 RECV PAGE.TXT
```

Ожидаемая последовательность:

```text
NETDRV: NE2000 init
NETDRV: ARP 10.0.2.2
NETDRV: ARP OK
NETDRV: TCP connect 10.0.2.2
NETDRV: RECV page saved
NETDRV: OK
```

Если TCP SYN-ACK не пришел:

```text
NETDRV: no TCP SYN-ACK
NETDRV: TCP connect failed
```

Это уже четко отделяет TCP-проблему от ARP.

## Runtime-проверка SEND

Обычный `python -m http.server` предназначен для GET и не является тестовым
приемником POST. Поэтому для SEND нужен отдельный POST-сервер.

Минимальный Python 3 сервер для Windows 7:

```python
from http.server import BaseHTTPRequestHandler, HTTPServer

class H(BaseHTTPRequestHandler):
    def do_POST(self):
        n = int(self.headers.get('Content-Length', '0'))
        data = self.rfile.read(n)
        with open('received.bin', 'wb') as f:
            f.write(data)
        self.send_response(200)
        self.end_headers()
        self.wfile.write(b'OK')

    def log_message(self, fmt, *args):
        print(fmt % args)

HTTPServer(('0.0.0.0', 8080), H).serve_forever()
```

На Windows 7:

```text
call python post_server.py
```

В Toy OS:

```text
exec NETDRV.EXE 10.0.2.2 SEND TEST.TXT
```

На Windows 7 после успешного теста должен появиться:

```text
received.bin
```

его размер должен совпадать с `TEST.TXT`.

## Примечание о сообщении QEMU

QEMU user-mode networking по умолчанию использует сеть `10.0.2.0/24`, адрес
гостевого видимого host обычно `10.0.2.2`, а гостю обычно назначается
`10.0.2.15`. Это соответствует данной конфигурации.

Сообщение SLIRP `Failed to send packet, ret: -1` само по себе не является
достаточным доказательством ошибки NETDRV: оно относится к callback отправки
пакета в backend SLIRP. В этой версии поэтому сначала проверяется факт ARP OK,
а затем отдельно TCP SYN-ACK.


## Дополнительное исправление после теста на QEMU/Windows 7

Если после ARP OK драйвер всё ещё получал `NETDRV: no TCP SYN-ACK`, причиной могла быть фильтрация unicast-кадра эмуляцией NE2000. В `netdrv.c` RCR теперь установлен в `0x14` (AB+PRO): broadcast и все unicast кадры принимаются. Это устраняет зависимость приёма SYN-ACK от внутренней PROM-копии MAC QEMU. В QEMU исходник NE2000 явно показывает: PRO=0x10 разрешает принимать все кадры, а обычный unicast дополнительно сравнивается с физическим адресом; приём возвращается с ошибкой, если устройство остановлено или RX-буфер заполнен.

Также сохранено исправление: при polling CURR драйвер использует START, а не STOP, чтобы не создавать окно потери входящего кадра.
