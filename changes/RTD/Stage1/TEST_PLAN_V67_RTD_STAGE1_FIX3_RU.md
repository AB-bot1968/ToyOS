# ToyOS v67 RTD Stage 1 FIX3 — план тестирования

## Цель

Исправить две связанные проблемы Stage 1:

1. detached RT task не получал CPU, пока shell находился внутри блокирующего `SYS_CONSOLE_READ`;
2. при завершении RT через ESC shell мог восстановиться из устаревшего общего EXE1 frame.

## Основные изменения

- введён отдельный `rt_shell_frame` для detached RT контекста;
- введён `rt_shell_waiting`, который разрешает переключение только когда shell действительно ждёт ввод в `SYS_CONSOLE_READ`;
- `SYS_CONSOLE_READ` при активном RT временно передаёт CPU RT task;
- timer IRQ больше не блокируется глобальным флагом syscall dispatch;
- timer не переключает произвольный kernel context: shell kernel frame переключается только при `rt_shell_waiting`;
- ESC по-прежнему устанавливает `rt_stop_requested` и завершает detached task безопасно.

## Фокусированный тест

```text
exec RTD.EXE SENSOR.EXE 20 20 3
```

Ожидается:

```text
RTD: started SENSOR.EXE period=20ms deadline=20ms priority=3
SENSOR: started as ordinary EXE1 (ESC to stop)
SENSOR: tick
SENSOR: tick
...
```

После `ESC`:

```text
SENSOR: ESC -> stopped
```

Shell должен продолжить работу без `EXC 14`.

## Статические проверки

```text
sh check_rtd_stage1_fix3.sh
```

## Ограничение

Параметры period/deadline/priority в Stage 1 остаются метаданными; политика реального времени ещё не изменяет round-robin механизм.
