# Stage 6.1 — журнал изменений

### Шаг 1
Добавлен отдельный аппаратный RT timebase 100 Hz.

### Шаг 2
Сохранён logical 50 Hz `timer_ticks` для обратной совместимости `SYS_TIMER_GET`.

### Шаг 3
RT scheduler переведён на обслуживание каждого IRQ0.

### Шаг 4
EXECMT оставлен на каждом втором IRQ0.

### Шаг 5
RT period/deadline/release/runtime budget переведены на 10 ms tick.

### Шаг 6
Добавлен `SYS_RT_TIME_GET=45`.

### Шаг 7
Добавлен `RTTIME.EXE` для совместной диагностики двух часов.

### Шаг 8
Обновлены regression tests и build pipeline.
