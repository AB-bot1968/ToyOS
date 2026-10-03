# ToyOS v67.11-B FIX7 — RTSTAT WATCH / BACKGROUND RT

## Исправленная ошибка

`rtstat watch` обновлялся раз в секунду, но его foreground shell loop выполнял только `SYS_RT_TIME_GET` и `SYS_CONSOLE_POLL`. Эти вызовы не отдавали процессор RT scheduler. В результате shell оставался текущим Ring-3 контекстом, а detached SENSOR-задачи не получали безопасной точки переключения; статистика могла оставаться неизменной.

## Исправление

Добавлен syscall:

```text
SYS_RT_YIELD = 49
```

`rtstat watch` после каждого прохода вызывает `sys_rt_yield()`.

Kernel:

1. если есть READY RT job — безопасно сохраняет текущий shell frame и переключается на RT task;
2. если READY job нет — выполняет `sti; hlt; cli`, позволяя следующему IRQ0 продвинуть RT clock и release state;
3. после возврата shell продолжает тот же `watch` loop.

Таким образом, период отображения остаётся 1 секунда, но RT tasks действительно получают CPU.

## F10

Успешное сообщение запуска остаётся ответственностью shell/F10 launcher. `RTD.EXE` после успешного `SYS_RT_START` молчит и немедленно завершает работу. Это исключает гонку вывода между RTD и shell.

Формат сообщения:

```text
RTD: started SENSOR1.EXE 10 10 5
```

и аналогично для остальных queued tasks.

## SENSOR

SENSOR остаётся полностью background-задачей. В `rt_sensor.c` отсутствует `SYS_CONSOLE_POLL`; завершение каждого job выполняется через `SYS_RT_WAIT`.

## Регистронезависимость

Сохраняется ASCII case-insensitive parsing команд и ключевых параметров:

```text
RTSTAT WATCH
rtstat watch
RtStAt WaTcH
```

и:

```text
RTSTAT STOP SLOT1
rtstat stop slot1
RtStAt StOp SlOt1
```

## Проверки

- `kernel.c` i386 freestanding compile — PASS
- `user_shell.c` i386 freestanding compile — PASS
- `rtd.c` i386 freestanding compile — PASS
- `rt_sensor.c` i386 freestanding compile — PASS
- `check_rt_watch_fix7.sh` — PASS

Полная штатная сборка W64DevKit должна быть выполнена в Windows/W64DevKit, поскольку этот Linux environment не является целевым W64DevKit toolchain.
