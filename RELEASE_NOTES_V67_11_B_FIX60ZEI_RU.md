# ToyOS v67.11-B FIX60ZEI — idle-aware heartbeat test contract

## Цель

FIX60ZEI создан строго от FIX60ZEH и не меняет runtime scheduler. Цель — исправить оставшийся недетерминированный FAIL `TESTMRT.TST` на строке 43, не снижая требования теста и не вмешиваясь в уже подтверждённую стабильную совместную работу 2 MT + 4 RT + console + SONAR.

## Точная причина строки 43

В FIX60ZEH строка 43 `TESTMRT.TST` была `ASSERT MT_HEARTBEAT_WAIT 2 12 800`. Перед ней выполнялся `CONSOLE_IDLE_TICKS 64`, который корректно создавал idle-shell окна. Однако затем `MT_HEARTBEAT_WAIT` переходил в обычный foreground busy-poll и только читал heartbeat/timer. При модели FIX60ZEF/ZEG foreground-команда владеет CPU, а detached MT получает slack в явно определённом idle-shell режиме. Поэтому ожидание heartbeat само прекращало предоставлять те окна, в которых heartbeat должен расти.

Это противоречие test harness, а не подтверждённая runtime-ошибка scheduler. Аналогичная потенциальная проблема существовала и на строке 46 с порогом 24.

## Исправление

Добавлен test-only ASSERT `MT_HEARTBEAT_IDLE_WAIT SLOT WANT LIMIT`. Он сохраняет тот же поиск PID, тот же `SYS_PROCESS_HEARTBEAT`, тот же `SYS_TIMER_GET` timeout и те же пороги. Отличается только способ ожидания:

- код `SYS_CONSOLE_READ = 2` создаёт реальное idle-shell окно;
- код `3` поглощается как штатный одноразовый redraw prompt;
- любой другой console-read result остаётся жёстким FAIL;
- между idle-return выполняется короткое Ring3-окно, чтобы PIT мог реализовать штатный bounded MT slack.

В `TESTMRT.TST` изменены только имена ASSERT на строках 43 и 46:

- `MT_HEARTBEAT_IDLE_WAIT 2 12 800`;
- `MT_HEARTBEAT_IDLE_WAIT 2 24 1200`.

Число строк файла сохранено равным 58, чтобы диагностические номера не сдвигались. Пороги 12/24 и timeout 800/1200 не уменьшены.

## Что не менялось

`src/kernel.c` байт-в-байт идентичен FIX60ZEH/FIX60ZEG, SHA-256: `3af4e5b2a9c8610eaee3efb1b533836dadf843e1b212922c9fee354c860dd001`. Следовательно, не менялись RT sweep, bounded MT slack, foreground ownership, RT/MT arbitration, prompt-redraw runtime contract, SONARLOG и `execmt SONARVWR.EXE`.

## Source guard

Механизм FIX60ZEH сохранён. Для `TESTMRT.TST` FIX60ZEI фактический fingerprint:

- bytes = `1692`;
- FNV-1a32 = `2456257148` (`0x9267827C`).

При runtime FAIL test runner по-прежнему печатает fingerprint реально прочитанного TST-файла.
