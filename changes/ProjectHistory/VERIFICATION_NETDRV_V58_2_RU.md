# NETDRV.EXE — проверка v58.2

## Найденные ошибки

### 1. Неверное поле IPv4 Total Length

В `ip_send_tcp()` переменная `total` уже была равна:

`20 байт IPv4 + 20 байт TCP + длина данных`.

Старый код записывал `20 + total`, поэтому IPv4 сообщал длину на 20 байт больше фактической TCP-полезной части.
Для SYN это давало Total Length=60 вместо 40. Сервер мог отбросить SYN, поэтому Toy OS доходил до `TCP connect failed`.

Исправлено на:

`put_be16(ip+2,total);`

### 2. Неверный NE2000 RCR

`RCR=0x01` — это SEP (сохранять ошибочные кадры), а AB (принимать broadcast) находится в бите `0x04`.
ARP-запрос является Ethernet broadcast, поэтому для минимального клиента требуется `RCR=0x04`.

Исправлено на:

`ne_w(NE_RCR,0x04u)`.

### 3. TCP data+ACK

При ожидании ACK сервер мог одновременно прислать данные HTTP. Старый код увеличивал `peer_seq`, а затем откладывал кадр.
При последующем разборе RECV последовательность уже не совпадала и ответ мог быть проигнорирован.

Теперь кадр откладывается без изменения `peer_seq`; RECV разбирает его и отправляет ACK после фактической обработки.

## Компиляционная проверка

Выполнены три независимых прохода:

```text
gcc -m32 -Os -ffreestanding -fno-pie -fno-stack-protector \
    -fno-asynchronous-unwind-tables -fno-unwind-tables -fno-builtin \
    -fno-tree-vectorize -fno-tree-slp-vectorize -mno-sse -mno-sse2 \
    -mno-mmx -mno-80387 -nostdinc -nostdlib -Werror -fsyntax-only src/netdrv.c
```

Все три прохода завершены без ошибок.

Проверены также:

- `src/NET.CFG` присутствует;
- `build.sh` копирует `src/NET.CFG` в `build/NET.CFG`;
- IPv4 Total Length использует полный размер `20+20+payload` ровно один раз;
- RCR содержит AB=`0x04`;
- старой формулы `20+total` в IPv4 Total Length нет.

## Runtime-проверка

Полный runtime QEMU + Windows 7 должен выполняться на машине разработчика, потому что текущая среда не содержит QEMU и Windows 7 HTTP-сервер.

### RECV

```text
exec NETDRV.EXE 10.0.2.2 RECV PAGE.TXT
```

Проверить:

```text
cat PAGE.TXT
```

### SEND

```text
exec NETDRV.EXE 10.0.2.2 SEND TEST.TXT
```

Для SEND нужен HTTP-сервер с POST-обработчиком.
