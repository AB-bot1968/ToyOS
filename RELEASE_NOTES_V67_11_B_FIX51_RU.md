# ToyOS v67.11-B FIX51 — Managed Process Recovery Lifecycle

FIX51 развивает стабильный FIX50: Recovery Manager теперь хранит наблюдаемое состояние managed restart, а Ring3 supervisor сообщает этапы остановки старого PID, запуска нового PID, heartbeat-подтверждения и ошибки восстановления.

Recovery считается успешным только после heartbeat нового PID. Restart budget FIX50 (3) сохранён. После исчерпания бюджета сохраняется переход SAFE. Новые filesystem I/O в kernel recovery path не добавлены.

Добавлены test-only RCONCE.EXE и TESTRST.TST. RCONCE детерминированно проверяет fault containment, освобождение намеренно оставленного PID-owned handle, создание нового PID и успешный heartbeat после restart. HANGWD проверяет bounded failure path до SAFE.

COMDRV/VGADRV/NETDRV и механизм port I/O не изменены. Scheduler policy не изменена.
