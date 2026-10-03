# ToyOS v67.8 — Stage 6.2: точное измерение release jitter

## База

Работа начата строго от зафиксированного состояния **ToyOS v67.7 — DELETE/LS FIX**.
Файловая подсистема, история команд, регистронезависимость shell/драйверов,
заставка и исправления v67.7 не пересобирались концептуально и не отменялись.

## Цель

Добавить измерительный механизм, позволяющий отличить:

1. номинальный период RT-задачи;
2. фактический момент, когда ядро обслужило очередной release;
3. фактический интервал между последовательными обслуженными release;
4. задержку от номинального release до его обслуживания;
5. задержку от release до первого dispatch RT-задачи.

Для Stage 6.1 один RT tick равен 10 ms. Поэтому для периода 10 ms идеальный
измеренный интервал равен одному RT tick.

## Изменение 1 — диагностические поля RT-задачи

В `struct sched_task` добавлены только измерительные поля:

- `rt_release_observed_tick` — последний фактический момент обслуживания release;
- `rt_release_last_tick` — предыдущий момент обслуживания release;
- `rt_release_interval_ticks` — последний фактический интервал;
- `rt_release_min_interval` / `rt_release_max_interval` — min/max интервала;
- `rt_release_samples` — число наблюдений;
- `rt_release_late_ticks` — задержка текущего release относительно номинального времени;
- `rt_release_max_late` — максимальная задержка;
- `rt_job_dispatch_tick` — первый dispatch текущего job;
- `rt_job_dispatch_latency` — dispatch tick минус номинальный release tick;
- `rt_job_dispatched` — флаг первого dispatch текущего job.

Эти поля не участвуют в выборе задачи, расчёте deadline, budget или release.

## Изменение 2 — точка измерения release

В `rt_release_jobs()` момент измерения берётся непосредственно перед
`rt_begin_job()`:

`rt_record_release_observation(task, scheduled_release, rt_time_ticks)`.

Поэтому измерение не выводится из заданного `period_ms`; оно получает реальный
`rt_time_ticks`, в котором ядро обнаружило и обслужило release.

Если IRQ0 или Ring-0 путь задержал обработку, например, номинальный release был
в tick 100, а обработан в tick 101, диагностический механизм показывает:

- `late_ticks = 1`;
- `dispatch_latency_ticks >= 1` в зависимости от фактического dispatch.

## Изменение 3 — измерение фактического интервала

Для двух последовательных обслуженных release вычисляется:

`observed[i] - observed[i-1]`

в modulo-2^32 арифметике.

Таким образом измерение не предполагает заранее, что период равен 1 tick.
Если фактические моменты обслуживания равны `100, 101, 103, 104`, интервалы будут
`1, 2, 1` ticks.

## Изменение 4 — измерение dispatch latency

В `rt_switch_to()` при первом переводе текущего job в RUNNING фиксируется:

`rt_time_ticks - rt_job_release_tick`.

Это позволяет отделить задержку обслуживания release от задержки фактического
получения CPU.

## Изменение 5 — новый read-only диагностический syscall

Добавлен `SYS_RT_JITTER_INFO = 46`.

Existing syscall IDs 1..45 не изменены.

Ring-3 получает 13 слов:

| Индекс | Значение |
|---:|---|
| 0 | job sequence |
| 1 | nominal period in RT ticks |
| 2 | scheduled release tick |
| 3 | observed release tick |
| 4 | last observed interval |
| 5 | minimum observed interval |
| 6 | maximum observed interval |
| 7 | current release lateness |
| 8 | maximum release lateness |
| 9 | first dispatch tick |
| 10 | first dispatch latency |
| 11 | first dispatch recorded flag |
| 12 | release observation samples |

Это расширение ABI является отдельным диагностическим syscall и не изменяет
существующие номера или форматы syscall.

## Изменение 6 — исполняемый JITTER.EXE

Добавлен обычный EXE1 `JITTER.EXE`.

Он выполняет 50 последовательных RT jobs, запрашивая измерения через syscall 46.
Для каждого наблюдения выводятся:

- scheduled release;
- observed release;
- interval ticks;
- interval ms;
- release lateness;
- dispatch latency.

В конце выводится SUMMARY с min/max/average interval и максимальными задержками.

При идеальном 10-ms периоде ожидается:

- `interval_ticks = 1`;
- `interval_ms = 10`;
- `max_release_late = 0`.

Программа не подменяет отклонение словом `PASS`: при отличии от номинала она
выводит фактические данные и завершает тестовым кодом 2.

## Исполняемые runtime-тесты

### Тест A — базовая точность 10 ms

В консоли ToyOS:

`exec rtd.exe jitter.exe 10 10 255`

затем `F10`.

Команда намеренно записана в нижнем регистре, чтобы одновременно проверять
регистронезависимость shell и запуска EXE.

Ожидаемый номинальный результат: 49 измеренных интервалов по 1 RT tick.
Допустимое отклонение не скрывается: оно печатается построчно.

### Тест B — влияние конкурирующего RT-переключения

Подготовить:

`exec rtd.exe jitter.exe 10 10 255`

`exec rtd.exe sensor1.exe 10 10 1`

затем `F10`.

В этом сценарии JITTER измеряет release/dispatch уже при наличии второй
периодической RT-задачи. Важен не заранее заданный PASS, а фактический SUMMARY
и построчные интервалы.

### Тест C — смешанный регистр команд

Повторить Тест A, например:

`ExEc RtD.ExE JiTtEr.ExE 10 10 255`

Это должно запускать тот же тест без изменения исходной строки аргументов.

## Автоматические проверки исходного кода

`check134.sh` выполняет:

1. host-исполняемый тест арифметики jitter;
2. freestanding-компиляцию `kernel.c`;
3. freestanding-компиляцию `user_shell.c`;
4. freestanding-компиляцию `rtd.c`;
5. freestanding-компиляцию `rt_jitter_diag.c`;
6. freestanding-компиляцию `rt_sensor.c`;
7. проверки наличия syscall 46 и всех диагностических полей;
8. проверки наличия `JITTER.EXE` в сборке;
9. проверки сохранения ASCII case-fold parser для команд RTD/LOADER/COMDRV/NETDRV/VGADRV.

Результат текущей проверки:

`CHECK134 PASS`

Полная `build.sh` требует проектный i686/W64DevKit toolchain и в текущей Linux
среде не объявляется успешно собранной только на основании host GCC.
