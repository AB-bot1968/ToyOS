# ToyOS v67.11-B FIX60ZEG — TEST contract + prompt redraw

## Цель

FIX60ZEG создан строго от зафиксированного FIX60ZEF. Главный приоритет — сохранить подтверждённую пользователем стабильность 4 RT 10/10 мс, двух MT-служб и интерактивной консоли.

## Что НЕ менялось

Политика RT/MT scheduler не менялась. Блоки `rt_switch_to_shell()`, `rt_wait()` и `sched_irq_tick()` байт-в-байт совпадают с FIX60ZEF. Distinct-slot RT sweep, foreground ownership и bounded MT slack оставлены без изменений.

## Исправление TESTMRT

В FIX60ZEF тест `TESTMRT.TST` требовал MT-progress и heartbeat, оставаясь внутри foreground-команды `test`. Это противоречило новой модели владения: во время обычной foreground-команды MT не обязан получать idle-shell slack.

В FIX60ZEG тест сначала создаёт контролируемые окна `CONSOLE_IDLE_TICKS`, то есть воспроизводит реальное состояние `toy0> _`, и только затем требует прежние значения `MT_PROGRESS` и `MT_HEARTBEAT_WAIT`. Пороговые значения прогресса и heartbeat не снижены.

## Автоматическое восстановление toy0>

`SONARVWR.EXE`, запущенный через `execmt`, остаётся асинхронной MT-задачей. Если detached MT/RT-процесс писал в консоль, ядро отмечает его slot как console-dirty. При завершении такого процесса формируется одноразовый redraw-event. Следующий `SYS_CONSOLE_READ` интерактивного shell возвращает уже существующий код 3, а `readline()` печатает `toy0> ` автоматически.

Этот механизм не выполняет context switch, не меняет CR3 и не вмешивается в scheduler.

## Регрессия

`TESTVWR.TST` после каждого из четырёх запусков viewer проверяет `ASSERT CONSOLE_REDRAW`. `TESTMRT.TST` проверяет MT-прогресс именно в реальном idle-shell режиме при 4 RT 10/10 мс. Общее число acceptance-тестов остаётся 45.
