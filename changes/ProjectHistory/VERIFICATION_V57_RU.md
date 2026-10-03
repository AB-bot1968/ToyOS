# Toy OS v57 — LOADER.EXE: автоматическая передача и запуск EXE1

## Цель

v57 добавляет пользовательскую программу `LOADER.EXE`, которая организует
автоматическую цепочку:

```text
exec LOADER.EXE 1 RECV FF.EXE
        |
        +--> COMDRV.EXE 1 RECV FF.EXE
        |
        +--> FF.EXE
```

Windows по-прежнему использует отдельную утилиту `win_sendfile.exe` для
передачи байтов по TCP в QEMU/COM1. Код Windows-утилиты и существующий формат
EXE1 не меняются.

## Почему LOADER использует очередь

Текущий EXE1 загружается в фиксированную область `0x00100000`. Поэтому простой
вложенный `SYS_EXEC_ARGS` не может вернуть управление старому LOADER после
загрузки COMDRV: образ LOADER был бы перезаписан образом COMDRV.

Поэтому LOADER не создаёт второй kernel loader. Он использует существующий
механизм `SYS_EXEC_QUEUE_ARGS` как безопасный chain-loader:

1. LOADER формирует две записи.
2. Первая запись запускает `COMDRV.EXE` с аргументами `PORT`, `RECV`, `FILE`.
3. Вторая запись содержит имя принятого `FILE.EXE`.
4. Ядро запускает вторую запись только после `SYS_EXIT(0)` первой.
5. При `SYS_EXIT(1)` очередь останавливается.

Это позволяет оставить существующий EXE1 ABI и не добавлять IPC.

## Команда

```text
exec LOADER.EXE 1 RECV FF.EXE
```

Поддерживается также:

```text
exec LOADER.EXE COM1 RECV FF.EXE
```

LOADER специально принимает только направление `RECV`.

## Изменения ядра

Изменены только две небольшие части поведения:

1. `SYS_EXEC_QUEUE_ARGS` разрешён из работающего Ring-3 EXE1.
2. Ненулевой `EBX` при `SYS_EXIT` останавливает активную очередь.

Остальные syscall numbers и формат EXE1 не изменены.

## Изменения COMDRV

COMDRV теперь передаёт код результата через:

```text
SYS_EXIT(EBX=0) — успешная передача;
SYS_EXIT(EBX=1) — ошибка.
```

При успешном RECV следующий элемент очереди запускается автоматически.

## Проверка

### 1. Собрать v57

В каталоге проекта:

```text
build.sh
```

В результате должны появиться:

```text
build/LOADER.EXE
build/COMDRV.EXE
build/HELLO.EXE
build/toy_os.img
```

### 2. Запустить QEMU

Использовать тот же проверенный запуск:

```text
d:\qemu\qemu-system-i386.exe -drive format=raw,file=toy_os.img -chardev socket,id=com1,host=127.0.0.1,port=5555,server=on,wait=off -device isa-serial,chardev=com1,iobase=0x3f8,irq=4
```

### 3. В shell

```text
exec LOADER.EXE 1 RECV FF.EXE
```

После этого на экране ожидается:

```text
LOADER: receiving EXE1 and starting it
COMDRV: receive complete, bytes=...
```

После закрытия TCP-потока и тайм-аута 1 секунда после последнего байта
COMDRV завершает работу, и очередь запускает:

```text
FF.EXE
```

Если вместо `FF.EXE` передавать уже собранный `HELLO.EXE`, но указывать:

```text
exec LOADER.EXE 1 RECV FF.EXE
```

то файл, полученный из Windows, будет сохранён под именем `FF.EXE`, а затем
именно `FF.EXE` будет запущен.

### 4. Проверка ошибки

Для проверки защитного поведения можно передать повреждённый/неверный EXE1.
Если COMDRV только записывает байты, а проверка EXE1 не проходит при попытке
запуска, kernel вернёт ошибку загрузки.

Для проверки именно защиты от ошибки COMDRV можно временно использовать
невалидный COM-порт:

```text
exec LOADER.EXE 9 RECV FF.EXE
```

В этом случае COMDRV завершается с кодом 1, и `FF.EXE` не запускается.

## Что намеренно не менялось

- формат EXE1;
- адрес `EXEC_LOAD_ADDR = 0x00100000`;
- FAT16;
- UART;
- syscall numbers 1..26;
- существующий `SYS_EXEC`;
- существующий shell parser;
- scheduler;
- Ring-3 сегменты;
- COMDRV timeout 10 секунд для первого байта;
- Windows `win_sendfile.exe`.

## Итог

v57 реализует автоматический путь:

```text
Windows
  |
  | TCP
  v
QEMU COM1
  |
  v
COMDRV.EXE
  |
  | FAT16
  v
FF.EXE
  |
  v
EXE1 execution
```

При этом LOADER остаётся обычной пользовательской программой, а изменения
ядра ограничены существующим механизмом очереди EXE1.
