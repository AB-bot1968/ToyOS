# ToyOS v67 RTD Stage 4.2 — fixed-priority arbitration

## 1. База

Этап выполнен от проверенной линии `ToyOS v67 RTD Stage 3 F10 FIX1` и API-компаратора Stage 4.1.

## 2. Изменение: priority подключён к `rt_pick_ready()`

Ранее Stage 3 выбирал READY RT-задачу по абсолютному `deadline`, а `priority` не участвовал в арбитраже.

В Stage 4.2 порядок стал:

1. больший числовой `priority` выигрывает;
2. при равном `priority` выигрывает более ранний абсолютный `deadline`;
3. при равных `priority` и `deadline` выигрывает меньший номер RT-слота.

Это подключает уже протестированный `rt_priority_compare()` непосредственно к планировщику.

## 3. Что не изменено

- `period` и release-логика;
- `deadline` и счётчик deadline miss;
- `SYS_RT_WAIT`;
- F10 preparation/launch flow;
- ESC и завершение RT-задачи;
- shell prompt `toy0>`;
- обычный EXE1 формат `SENSOR.EXE`;
- EXE1 loader;
- EXECMT;
- PIT 50 Hz;
- VGA/text transition.

## 4. Тестируемые правила

Примеры:

- priority 7 против priority 3 → выбирается 7;
- priority 3 против priority 7 → выбирается 7;
- одинаковый priority, deadline 10 против 20 → выбирается deadline 10;
- одинаковые priority/deadline → выбирается меньший слот.

## 5. Следующий шаг

После runtime-подтверждения Stage 4.2 можно переходить к отдельному шагу priority/preemption behavior и затем к связи priority с deadline policy.
