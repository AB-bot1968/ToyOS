# ToyOS v67 RTD Stage 3 F10 — отложенный запуск RT-задач

## 1. Основание

Исходная база: `ToyOS v67 RTD Stage 3 FIX4`.

Цель изменения: устранить необходимость вводить следующую `exec RTD.EXE ...` в момент, когда уже работающая RT-задача активно использует CPU. Команда RTD теперь сначала формирует отложенную заявку в shell. Запуск выполняется после `F10`.

## 2. Изменение интерфейса shell

Команда:

```text
exec RTD.EXE SENSOR.EXE PERIOD DEADLINE PRIORITY [DELAY]
```

больше не вызывает `SYS_EXEC_ARGS` немедленно.

Вместо этого shell сохраняет 48-байтный EXE1 argument block в очереди `pending_rt_args`.

После успешного разбора команда печатает:

```text
RTD: task prepared: ...
RTD: enter next RT command or press F10 to start queued task(s)
```

## 3. Очередь заявок

Добавлены:

```c
#define RT_PENDING_MAX 4u
static char pending_rt_args[RT_PENDING_MAX][EXEC_ARG_BLOCK_SIZE];
static uint32_t pending_rt_count;
```

Максимум четыре подготовленные заявки соответствует существующему `RT_MAX_TASKS=4`.

Заявки содержат тот же argument block, который ранее сразу передавался `RTD.EXE`. ABI `SYS_EXEC_ARGS` не изменён.

## 4. Клавиша F10

Для PS/2 Set-1 используется scancode `0x44`.

В IRQ1 он переводится во внутреннее событие:

```c
#define KEY_EVENT_F10 0xf0u
```

Это значение не является обычным печатным символом.

`SYS_CONSOLE_READ` возвращает shell специальный результат `4` (`SHELL_READ_F10`), а `readline()` передаёт событие в `shell_loop()`.

## 5. Запуск очереди

`rt_launch_pending()` обходит подготовленные заявки и передаёт каждую штатному:

```c
sys_exec_args("/RTD.EXE", pending_rt_args[i]);
```

Таким образом фактическое создание RT-задачи по-прежнему выполняет штатный `RTD.EXE`/`SYS_RT_START` путь. Планировщик RT не переписывается.

При частичной ошибке уже запущенные заявки удаляются из очереди, а не запущенные остаются для повторной попытки F10.

## 6. ESC и SENSOR

Механизм `ESC -> SENSOR: ESC -> stopped -> toy0>` не изменён.

`SENSOR.EXE` остаётся обычным EXE1.

## 7. Совместимость

PIT 50 Hz, `period/deadline/priority`, `SYS_RT_WAIT`, EXECMT, обычный EXE1 loader и существующие syscall ABI не изменены.

Изменено только поведение shell-команды RTD: запуск стал отложенным до F10.
