# ToyOS v67.11-B FIX49 — Persistent Fault/Recovery History

FIX49 построен строго от FIX48B CURRENT STABLE.

Добавлена достоверная история recovery/safety поверх существующего FIX41 EVENT.LOG. Формат записи EVENT.LOG не изменён: sequence/tick/PID/type/reason/status/vector/error остаются совместимыми. Новые типы: 100 NORMAL, 101 DEGRADED, 102 SAFE, 103 RESTART, 104 software-watchdog timeout, 105 restart-budget exhausted. Существующие FAULT (reason=2) и watchdog stop (reason=3) не переопределены.

Новый SYS_RECOVERY_EVENT=67 только добавляет запись в RAM ring. Он не выполняет FAT16 I/O, не меняет scheduler, не запускает recovery и не переводит SAFE state. Переходы SAFE state журналируются ядром в RAM. Supervisor добавляет RESTART/WATCHDOG/BUDGET markers в известных безопасных Ring3 точках.

Команда `recoverylog` показывает только recovery/safety историю из RAM; `recoverylog disk` — из сохранённого EVENT.LOG. Сохранение остаётся явной Ring3 shell-операцией `eventlog save`, поэтому принцип FIX41 о запрете FAT16-записи из IRQ/exception/RT/kernel dangerous path сохранён.

Добавлен TESTRCV.TST. Усилены TESTSUP.TST, TESTWD.TST и TESTPOL.TST.
