# ToyOS v67.11-B FIX60O — EXECMT / serial boundary

Основа: FIX60N. Физический тест FIX60N: SONARSIM показывал только `request=1 response=17`, а ToyOS оставался на строке `execmt: loading 1 independent tasks...`.

Исправлено:
1. Отменена небезопасная немедленная активация FIX60N. `SYS_EXECMT` только полностью загружает READY-задачи и возвращает PREPARED session. Первый runnable commit выполняется после того, как shell уже вывел `execmt: started...` и `toy0>`. Переключение контекста по-прежнему делает только IRQ0.
2. Добавлен `SYS_EXECMT_COMMIT=75`: state-only PREPARED->RUNNABLE без context switch. Он нужен для автоматических тестов, чтобы тестовый runner использовал ту же transition, но не выдавал READY metadata за реальное выполнение.
3. Добавлен SONARTST.EXE — тестовая сборка того же sonardrv.c. Через существующий UART injection hook она подаёт корректный 17-byte response и проходит настоящую цепочку parse/decode/publish/heartbeat/next cycle.
4. Добавлен TESTSONO.TST: требует MT progress, минимум 2 Data Channel publications и минимум 2 heartbeat.
5. SONARSIM Windows переведён на bounded overlapped COM I/O. `tx=17` печатается только после завершённого WriteFile; зависшая передача даёт `tx=TIMEOUT`. Сохранена CRC resynchronization byte stream.

Не изменены: syscall 13/14, политика I/O ports, RT priority, MT quantum 20 ms, Modbus wire format, Data Channel ABI, production SONARDRV polling transport и masked IRQ4.
