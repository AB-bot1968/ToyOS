# ToyOS v67 RTD Stage 5.2 FIX1 — обработка проверки deadline miss

## Основание

Базовая версия: `ToyOS v67 RTD Stage 5.2`.

## Наблюдение runtime

При тесте:

`exec RTD.EXE RTDMISS.EXE 1000 40 7`

`exec RTD.EXE SENSOR1.EXE 20 20 3`

`F10`

нужно различать нормальный порядок первого dispatch и поведение после `deadline miss`.

Первый запуск `RTDMISS.EXE` сам по себе не является ошибкой: обе заявки становятся READY не обязательно в один и тот же момент. Требование Stage 5.2 начинается с момента, когда текущая RT job действительно становится `MISSED`.

## Исправления

1. Добавлена wrap-safe функция `rt_deadline_before()`.
2. `rt_pick_ready_class()` теперь использует её для сравнения абсолютных deadline при равных priority.
3. `RTDMISS.EXE` выводит `dispatch`, `slot`, `priority`, `switches`, что позволяет отличить фактический scheduler dispatch от простого порядка строк.
4. Добавлен интеграционный host-тест, повторяющий ключевой сценарий Stage 5.2:
   - `RTDMISS` имеет `priority=7`, но `MISSED`;
   - `SENSOR1` имеет `priority=3` и `ON-TIME`;
   - выбирается `SENSOR1`;
   - если on-time задача недоступна, `RTDMISS` выбирается как best-effort.

## Политика Stage 5.2

`deadline miss` не уничтожает текущую job.

`MISSED` job уступает любой `ON-TIME READY` job независимо от priority.

Если on-time READY jobs нет, используется существующее правило priority/deadline/slot внутри best-effort класса.
