# ToyOS v67 — тест-план RTD stage 1

## 1. Сборка

Проверить компиляцию:

- `src/kernel.c`
- `src/user_shell.c`
- `src/rtd.c`
- `src/rt_sensor.c`

и создание:

- `build/RTD.EXE`
- `build/SENSOR.EXE`
- `build/disk.img`

## 2. EXE1 ABI

`check_rtd_stage1.sh` проверяет:

- наличие обоих EXE1;
- magic `EXE1`;
- entry `0x00100000`;
- наличие файлов в FAT16 root;
- размер kernel <= 64 KiB;
- наличие `SYS_RT_START=38`;
- наличие detached RT state и RT metadata.

## 3. Ручной runtime test

После загрузки ToyOS выполнить:

```text
exec RTD.EXE SENSOR.EXE 50 50 3
```

Ожидается:

```text
RTD: started SENSOR.EXE period=50ms deadline=50ms priority=3
SENSOR: started as ordinary EXE1
```

Shell должен вернуть prompt `toy0>` и продолжить принимать команды.

При работающем `SENSOR.EXE` должны периодически появляться строки `SENSOR: tick`, что подтверждает preemption IRQ0 и возврат управления shell.

## 4. Регрессия

Проверить отдельно:

- `exec HELLO.EXE`;
- `execmt MT01.EXE MT02.EXE`;
- `execq 2 QPASS.EXE`;
- `pit-test`;
- `syscall-test`;
- NETDRV/VGADRV и существующие FAT16/path tests.

## 5. Негативные тесты

```text
exec RTD.EXE SENSOR.EXE
exec RTD.EXE SENSOR.EXE 0 50 3
exec RTD.EXE SENSOR.EXE 50 60 3
exec RTD.EXE SENSOR.EXE 50 50 256
```

Во всех случаях RTD должен отказаться от запуска, а shell не должен зависнуть.

## Дополнительные тесты FIX1/2

### T-RTD-ERR-01 — сохранение числовых параметров
Команда:
`exec RTD.EXE SENSOR.EXE 20 20 3`

Ожидание: RTD не возвращает `4294967294` (`-2` / `EXE_ERR_NAME`) из-за обнуления структуры запроса; параметры передаются в ядро как `20/20/3`.

### T-RTD-ESC-01 — завершение SENSOR по ESC
1. Запустить `exec RTD.EXE SENSOR.EXE 20 20 3`.
2. Убедиться, что SENSOR выполняется и shell остаётся доступным.
3. Нажать `ESC`.

Ожидание: detached RT-задача SENSOR завершается, shell возвращается/остаётся активным. SENSOR использует существующий `SYS_CONSOLE_POLL` и `SYS_EXIT`; если в момент `ESC` SENSOR находится вне исполнения, kernel сохраняет запрос остановки и завершает RT-задачу на ближайшем IRQ0.
