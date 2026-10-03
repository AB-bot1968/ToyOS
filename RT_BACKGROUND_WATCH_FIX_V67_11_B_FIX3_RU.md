# ToyOS v67.11-B-FIX3 — исправление фоновых SENSOR и RTSTAT WATCH

## Найденные ошибки

1. `SENSOR1..SENSOR4` печатали `started` и `ESC` независимо от режима verbose.
   Поэтому внутренний quiet-запуск F10 не был действительно тихим.
2. `rtstat watch` работал в foreground shell-контексте, но RT scheduler сохранял
   shell frame только для `SYS_CONSOLE_READ`. Когда SENSOR вызывал `SYS_RT_WAIT`
   во время `watch`, при отсутствии другого READY RT job scheduler мог вернуть
   выполнение в тот же RT-контекст вместо foreground shell. В результате RT-задача
   могла остаться в состоянии BLOCKED, продолжая выполняться, а статистика
   переставала быть достоверным потоком завершённых периодов.
3. `watch` был переведён на 10 RT ticks (100 ms), хотя пользовательский контракт
   команды — обновление раз в секунду. Теперь обновление выполняется каждые
   100 RT ticks (1 s) по единому RT clock.

## Исправление

- SENSOR1..SENSOR4 полностью лишены console-verbose режима;
- перед переходом shell -> RT сохраняется актуальный Ring-3 shell frame;
- RT `SYS_RT_WAIT` возвращает управление foreground shell даже если shell не
  находится внутри `SYS_CONSOLE_READ`;
- специальный EAX=2 используется только для продолжения заблокированного
  `SYS_CONSOLE_READ`;
- `rtstat watch` использует `SYS_RT_TIME_GET` и обновляется каждые 1 s (100 RT ticks);
- ABI/syscall 1..48 не изменён;
- FAT16 LBA 512 сохраняется;
- регистр команд shell остаётся нечувствительным к ASCII case.
