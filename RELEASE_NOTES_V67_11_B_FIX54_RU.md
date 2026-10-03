# FIX54 — усиление инфраструктуры перед supervised driver

FIX54 закрывает подтвержденный технический долг FIX53 перед реализацией UART/Modbus/Sensor Channel.

1. Устранен обход SAFE через исторический syscall 18 (SYS_MT_START).
2. Recovery Manager больше не принимает ключевые переходы lifecycle только на доверии к Ring-3 сообщению.
3. MT STOP ONE/ALL унифицирован с process-result lifecycle: причина STOPPED сохраняется, handles освобождаются, heartbeat очищается.
4. Исправлена семантика TST: RUN не считается доказательством свойства и не увеличивает PASS; WAIT LAST дает PASS только при реальном результате процесса.
5. Добавлен TESTINF.TST.

Архитектура syscall 13/14 и свободного доступа Ring-3 приложений ко всем I/O-портам не изменялась.
