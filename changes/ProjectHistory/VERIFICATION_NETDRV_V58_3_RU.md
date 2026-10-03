# NETDRV.EXE1 v58 — исправление RX/TX №3

## Причина очередного `connect failed`

Предыдущие версии уже исправляли RX ring, IPv4 Total Length и обработку TCP DATA+ACK.
При анализе поведения QEMU NE2000 была обнаружена ещё одна важная особенность:
remote DMA должен завершаться по флагу `ISR.RDC`, а передача минимального Ethernet
кадра должна действительно содержать все 60 байт, а не только менять значение
TBCR. В новой версии обе операции выполняются явно.

Дополнительно приём NE2000 переведён в режим `PRO + AB` (`RCR=0x14`). Это делает
драйвер независимым от различий в фильтрации физического MAC между версиями QEMU.

## Изменения

1. `ne_wait_rdc()` ждёт `ISR.RDC` после каждой remote-DMA операции.
2. Перед remote DMA контроллер переводится в `START`, затем задаются RSAR/RBCR.
3. `ne_ring_read()` умеет читать пакет через границу `PSTOP -> PSTART`.
4. RX header проверяет `RXOK`.
5. TX кадры короче 60 байт реально дополняются нулями.
6. IPv4 RX проверяет `Total Length` и checksum.
7. TCP SYN+ACK проверяет ACK = `SYN sequence + 1` и отвергает RST.
8. `pending_valid` очищается перед новым TCP соединением.
9. Сохранены предыдущие исправления `BNRY=PSTART-1`, IPv4 Total Length и TCP DATA+ACK.

## Почему сообщение QEMU `Slirp: Failed to send packet, ret: -1` важно

В QEMU callback отправки пакета из SLIRP в эмулируемую NIC возвращаемое значение `<0`
означает, что пакет не был принят backend/NIC callback. В исходнике QEMU для NE2000
callback может вернуть `-1`, в частности когда устройство остановлено или RX ring
считается заполненным. Поэтому это сообщение нельзя автоматически считать ошибкой
Windows web-сервера.

После исправления драйвера первым проверяется именно RX ring и состояние NE2000.

## Компиляционная проверка

В W64DevKit x86 выполнить:

```text
./check59.sh
./check60.sh
```

`check59.sh` выполняет три компиляционных прохода NETDRV:

1. `-fsyntax-only`;
2. обычный объектный файл;
3. объектный файл с `-fno-omit-frame-pointer`.

`check60.sh` проверяет ключевые исправления RX/TX статически.

## QEMU

Рекомендуемый запуск:

```text
qemu-system-i386 -hda build/toy_os.img -m 16M -netdev user,id=n0 -device ne2k_isa,netdev=n0,mac=52:54:00:12:34:56
```

Стандартная QEMU user-mode сеть использует `10.0.2.0/24`, гостевой адрес обычно
`10.0.2.15`, а адрес виртуального host/gateway — `10.0.2.2`.

## RECV

На Windows 7 сервер должен слушать порт, указанный в `src/NET.CFG`.
Например:

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

После `./build.sh` в Toy OS:

```text
cat NET.CFG
exec NETDRV.EXE 10.0.2.2 RECV PAGE.TXT
cat PAGE.TXT
```

Ожидаемая цепочка:

```text
ARP -> SYN -> SYN/ACK -> ACK -> GET -> HTTP DATA -> FIN/ACK
```

## SEND

Создать в Toy OS небольшой файл:

```text
copy ...
```

или использовать существующий тестовый файл, затем:

```text
exec NETDRV.EXE 10.0.2.2 SEND TEST.TXT
```

Для SEND сервер должен поддерживать HTTP POST. Простой `SimpleHTTPServer` Python 2
обычно обслуживает GET, но POST может вернуть `501 Not Implemented`; это не является
признаком ошибки TCP/NE2000.

## Если после этой версии остаётся `connect failed`

Нужно сохранить полный текст QEMU, начиная с первого запуска, и полный вывод Toy OS
от:

```text
NETDRV: NE2000 init
```

до:

```text
NETDRV: TCP connect failed
```

Особенно важны сообщения `Slirp: Failed to send packet` и момент их появления.
Это позволит отличить отказ RX ring NE2000 от отказа host-side socket Windows 7.
