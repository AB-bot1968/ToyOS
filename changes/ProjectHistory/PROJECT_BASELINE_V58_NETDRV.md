# Toy OS v58 + NETDRV.EXE

Это расширение базовой v58 без изменения `src/kernel.c`.

## Состав

- `src/netdrv.c` — Ring-3 NETDRV.EXE1;
- `src/user_shell.c` — передача IP/DIRECTION/FILE через существующий `SYS_EXEC_ARGS`;
- `NET.CFG` — фиксированное имя конфигурации;
- `build.sh` — сборка и включение EXE1/CFG в FAT16;
- `check59.sh` — тройная компиляционная проверка.

## Интерфейс

```text
exec NETDRV.EXE IP SEND FILE.TXT
exec NETDRV.EXE IP RECV FILE.TXT
```

Протокол — HTTP/1.0 поверх TCP/IPv4.

## Исправление v58.1 — RX ring NE2000

Исправлена инициализация BNRY: теперь при PSTART=0x46 используется BNRY=0x45. Предыдущее значение 0x7F делало первый расчёт next равным 0x46, совпадающим с CURR, поэтому polling RX ошибочно видел пустое кольцо и ARP/TCP ответы не обрабатывались.
