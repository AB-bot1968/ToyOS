# ToyOS v67.10 — Stage 6.2 RT MONITOR / STATISTICS

## База

v67.10 продолжает зафиксированную рабочую базу v67.9 SENSOR RT OBSERVABILITY.
Существующие syscall 1..47, RT scheduler policy и формат EXE1 не изменяются.
Добавлен только новый диагностический syscall `SYS_RT_STATS = 48`.

## Главная цель

Вывод SENSOR1..SENSOR4 больше не является частью обычного измерительного пути.
По умолчанию sensor-задача печатает только строку запуска и сообщения об ошибке/ESC.
Kernel самостоятельно собирает статистику при завершении каждого RT job через
`SYS_RT_WAIT`.

Это позволяет выполнять нагрузочные тесты без измерительного искажения от частого
`SYS_CONSOLE_WRITE`.

## Команды shell

Все команды и подкоманды регистронезависимы.

```text
rtstat
rtstat compare
rtstat watch
rtstat dump
rtstat reset
```

### `rtstat`

Показывает накопленную статистику по активным RT task и по слотам, для которых
уже есть измерения.

Поля:

- `jobs` — число завершённых и измеренных RT jobs;
- `interval avg/min/max` — фактический интервал между serviced releases;
- `jitter avg/min/max` — signed `interval - requested period`;
- `dispatch avg/min/max` — release-to-first-dispatch latency;
- `cpu avg/min/max` — CPU ticks, фактически использованные job;
- `wall avg/min/max` — dispatch-to-completion wall span;
- `miss` — накопленные deadline misses;
- `skips` — накопленные skipped releases.

Один RT tick равен 10 ms.

### `rtstat compare`

Выводит SENSOR1..SENSOR4/RT tasks в едином формате. Автоматического рейтинга
или `BEST/WORST` нет: команда показывает измеренные числа без оценочного вывода.

### `rtstat watch`

Обновляет сводную таблицу ровно раз в 1 секунду по RT clock (100 RT ticks). `ESC` завершает режим.
Сбор статистики продолжается kernel-side независимо от частоты обновления экрана.

### `rtstat dump`

Показывает последние до 8 завершённых jobs каждого RT task из небольшого
kernel-side ring buffer в хронологическом порядке. Для каждой записи выводятся:
sequence, interval, signed jitter, dispatch latency, CPU, wall, miss и skips.

### `rtstat reset`

Обнуляет накопленную статистику и ring buffers, но не останавливает RT tasks.
Следующие jobs снова начинают формировать измерения.

## SENSOR1..SENSOR4 — только background mode

`SENSOR1..SENSOR4` являются detached/background RT-задачами по определению.
Параметр `VERBOSE` удалён из интерфейса RTD и больше не существует.

SENSOR не пишет диагностические строки в интерактивную консоль и не читает
обычные клавиатурные байты. Все измерения доступны через `rtstat`,
`rtstat compare`, `rtstat watch` и `rtstat dump`.

## Архитектура достоверности

1. RT scheduler продолжает работать с прежним RT clock 100 Hz.
2. Kernel фиксирует timing независимо от console output.
3. Aggregate statistics обновляются при `SYS_RT_WAIT`, после фактического job.
4. Ring buffer хранит только последние 8 завершённых jobs на RT slot.
5. `rtstat watch` не изменяет scheduler policy.
6. `rtstat compare` не сортирует и не ранжирует задачи.

## ABI

- `SYS_RT_EXEC_INFO = 47` сохранён без изменений.
- `SYS_RT_STATS = 48` — новый диагностический интерфейс.
- Syscall 1..47 не перенумеровывались.
- EXE1 ABI и существующий `SYS_RT_START = 38` не изменены.
