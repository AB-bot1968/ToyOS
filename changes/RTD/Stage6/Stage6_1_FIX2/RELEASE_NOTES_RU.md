# ToyOS v67 RTD — Stage 6.1 FIX2

## Назначение

Исправить запуск короткопериодической RT-задачи с `period=10 ms` и сохранить
совместимость со стабильной базой v67.5.3 / RTD Stage 5.3 и последующей
верифицированной веткой Stage 4.4 / Stage 5.x.

## Симптом

После команды:

    exec RTD.EXE SENSOR1.EXE 10 10 3

и запуска подготовленной задачи через F10 на консоли появлялось только
сообщение `task 1 started`, а пользовательская задача могла не получить CPU.
В результате `SENSOR1.EXE` не доходил до первого `tick`.

## Корень проблемы

Stage 6.1 перевёл аппаратный PIT на 100 Hz, то есть один RT tick = 10 ms,
но `SYS_CONSOLE_READ` shell первоначально выбирал только класс `ON-TIME READY`.

Для предельного случая `deadline=10 ms` до первого dispatch мог пройти один
RT tick. Тогда job уже имела `rt_job_missed=1`, находилась в состоянии READY,
но shell внутри `SYS_CONSOLE_READ` продолжал выбирать только `rt_pick_ready()`.

Следствием было состояние:

    READY + MISSED + shell waiting

без безопасного пути запуска best-effort job из самого блокирующего shell
syscall. IRQ0 при этом не мог исправлять ситуацию, если сам прервал Ring-0
код: Stage 6.1 FIX1 намеренно запрещает такой context switch.

## Исправление

В `SYS_CONSOLE_READ` добавлена последовательность:

    rid = rt_pick_ready();
    if (rid < 0) rid = rt_pick_ready_missed();

Это означает:

1. сначала всегда выбирается своевременная RT job;
2. если своевременной READY job нет, shell может безопасно передать CPU
   просроченной best-effort job;
3. прежний запрет context switch из IRQ0, прервавшего Ring 0, сохраняется.

## Дополнительное изменение диагностического SENSOR1/SENSOR2

`src/rt_sensor_diag.c` теперь использует `SYS_RT_TIME_GET` для ожидания следующего
RT tick. Это делает диагностический вывод соответствующим новой 10-ms RT шкале
и не маскирует Stage 6.1 особенностью старого 50-Hz `SYS_TIMER_GET`.

Обычный `SENSOR.EXE` не изменён.

## Совместимость

Не изменены:

- `SYS_TIMER_GET` и логический 50-Hz системный таймер;
- `ESC` и `SENSOR: ESC -> stopped`;
- F10 и очередь подготовленных RT-команд;
- `SYS_RT_WAIT`;
- `period/deadline/priority` семантика предыдущих этапов;
- обычный EXE1 формат;
- EXECMT;
- VGA/text shell.
