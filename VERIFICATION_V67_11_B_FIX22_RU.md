# Проверка FIX22 — MTSTAT / диагностика RT+MT

Цель FIX22 — измерить проблему, не менять scheduler policy. Сами MT задачи не
пишут статистику в console. Все значения снимаются ядром.

## Поля MTSTAT

- `DISPATCH` — сколько раз ядро фактически передало управление данному MT slot.
- `QUANTA` — сколько диагностических 20-ms CPU интервалов накоплено задачей
  (2 фактических CPU ticks по 10 ms). Это измеритель, а не scheduler control.
- `CPU_TICKS` — фактические 10-ms интервалы, завершённые во время исполнения MT.
- `STATE` — READY/RUN/BLOCK/STOP/EXIT.

При работающей задаче счётчики не обязаны расти строго одинаково между строками,
но `CPU_TICKS` должен быть монотонным. Для CPU-bound MT задач примерно
`QUANTA ~= CPU_TICKS/2`.

## Тест 1 — регрессия FIX21

1. Запустить SENSOR1..SENSOR8 через RTD/F10.
2. `rtstat watch`, дождаться нескольких обновлений, ESC.
3. Убедиться `rtstat`, что RT tasks остались активны.
4. Проверить `rtdata SLOT1` и `rtdata SLOT8`.

Ожидание: FIX21 WATCH/ESC не сломан.

## Тест 2 — MT без RT (контроль)

После `rtstat stop`/чистой загрузки без RT:

    execmt MT01.EXE MT02.EXE MT03.EXE MT04.EXE
    mtlist
    mtstat
    mtdata mt1
    mtdata mt2
    mtdata mt3
    mtdata mt4
    mtstat watch

Оставить WATCH минимум на 10 секунд. Записать значения MT1..MT4 на первом и
последнем экране. ESC должен вернуть shell.

Ожидание: все четыре MT получают DISPATCH и CPU_TICKS; MTDATA каждой задачи
становится `MTxx:RUN;`. При CPU-bound workload счётчики всех четырёх должны
продвигаться, без длительного полного зависания одного slot.

## Тест 3 — 4 RT + 4 MT

1. `mtstop all`.
2. Запустить SENSOR1..SENSOR4 (рекомендуемые текущие параметры RTD оставить теми
   же, что использовались в предыдущих тестах).
3. Запустить MT01..MT04.
4. `mtstat watch` на 20–30 секунд.
5. ESC; выполнить `rtstat`, `mtstat`, `mtdata mt1`..`mtdata mt4`.

Записать прирост DISPATCH/QUANTA/CPU_TICKS за интервал и RT MISS/SKIP.

## Тест 4 — 8 RT + 4 MT (главный тест неисправности)

1. `mtstop all`, затем запустить SENSOR1..SENSOR8 через F10.
2. Убедиться `rtstat`, что 8 RT slots активны.
3. `execmt MT01.EXE MT02.EXE MT03.EXE MT04.EXE`.
4. Сразу `mtstat`, затем `mtstat watch` минимум 30 секунд.
5. ESC; `mtstat`; затем `mtdata mt1`..`mtdata mt4`.
6. Не останавливая MT, выполнить `rtstat stop` (все RT).
7. Через 5–10 секунд снова `mtstat` и `mtdata mt1`..`mtdata mt4`.

Ключевая диагностика:
- если при 8 RT `DISPATCH=0` и `CPU_TICKS=0`, а после `rtstat stop` они начинают
  расти — MT starvation находится в пути RT -> shell/MT scheduler;
- если DISPATCH растёт, но CPU_TICKS почти нет — MT получает входы, но быстро
  вытесняется до накопления CPU времени;
- если CPU_TICKS растёт нормально, но MTDATA остаётся `no data` — исследовать
  SYS_MT_DATA/task startup отдельно;
- если только часть MT slots растёт — исследовать round-robin selection;
- если после остановки RT все MT резко начинают расти — сохранить значения до
  и после: это особенно важно для локализации.

## Тест 5 — 8 RT + 8 MT

Повторить главный тест с MT01..MT08. Проверить, что нет slot, у которого
счётчики постоянно стоят, пока остальные растут. Не делать вывод по одному
экрану: наблюдать минимум 30 секунд.

## Тест 6 — case-insensitive / ESC

Проверить:

    MTSTAT
    mtstat
    MtStAt WaTcH

Во всех вариантах команда должна распознаваться. В WATCH нажать ESC 10 раз в
отдельных входах; после каждого выхода shell должен принимать команды, RT/MT
задачи не должны самопроизвольно останавливаться.

## Тест 7 — новая EXECMT session

1. Запустить MT01..MT04, дождаться ненулевых counters.
2. `mtstop all`.
3. Запустить новую EXECMT session.
4. Сразу проверить `mtstat`.

Ожидание: статистика новой session начинается заново; старые counters не
приписываются новым MT slots.

## Что прислать для анализа

Наиболее полезны четыре снимка:
A) `mtstat` после 10 s: 4 MT, RT=0;
B) `mtstat` после 30 s: 4 MT + 4 RT;
C) `mtstat` после 30 s: 4 MT + 8 RT;
D) тот же запуск через 10 s после `rtstat stop` всех RT.

Для C также нужны `rtstat` и результаты `mtdata mt1`..`mtdata mt4`.
По этим данным можно отличить реальную нехватку CPU от ошибки перехода RT->MT,
не меняя scheduler вслепую.
