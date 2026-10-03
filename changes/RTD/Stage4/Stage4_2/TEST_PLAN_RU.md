# План тестирования Stage 4.2

## Шаг 1 — host unit tests

```text
sh check_rtd_stage4_2.sh
```

Ожидается:

```text
RTD Stage 4.2 priority arbitration checks passed
```

## Шаг 2 — проверка старых focused tests

Выполнить Stage 1–3 RTD checks из `build.sh`. Они не должны менять поведение ESC, SYS_RT_WAIT, F10 и shell context.

## Шаг 3 — runtime preparation

Подготовить две RT-задачи через F10:

```text
exec RTD.EXE SENSOR.EXE 20 20 3
exec RTD.EXE SENSOR.EXE 20 20 7
F10
```

При одинаковом release/deadline точка выбора должна отдавать предпочтение priority 7.

## Шаг 4 — equal-priority EDF tie-break

```text
exec RTD.EXE SENSOR.EXE 20 20 5
exec RTD.EXE SENSOR.EXE 40 40 5
F10
```

При одновременной готовности сначала должна выбираться job с более ранним абсолютным deadline.

## Шаг 5 — ESC regression

После runtime-теста нажать `ESC` и убедиться, что завершение и возврат `toy0>` работают как в FIX2/F10 FIX1.

## Ограничение

Реальный boot/runtime в текущем контейнере не выполняется без `qemu-system-i386`.
