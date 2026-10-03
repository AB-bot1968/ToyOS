# ToyOS v67.11-B FIX60R — SONARLOG lifecycle

Основа: принятый стабильный FIX60Q. Рабочий `SONARDRV.EXE`, Modbus, UART polling и QEMU TCP transport не изменялись.

Исправлено:
- SONARLOG больше не завершается, если Data Channel 0 ещё не создан в момент первого запуска; reader открывается повторно до появления producer.
- при invalidated reader выполняется повторное открытие, а не завершение logger.
- Data Channel reader теперь имеет owner PID и освобождается ядром при normal/fault/forced/watchdog lifecycle cleanup; устранена утечка reader slots при `mtstop`.
- журнал закреплён за абсолютным `/SONAR.LOG`, а не за текущим cwd shell.
- короткий/неправильного размера файл и состояние с двумя недействительными headers восстанавливаются в пустой корректный циклический журнал 4160 байт.
- SONARLOG публикует heartbeat после готовности reader и после каждого committed telemetry record.
- `TESTLGR.TST` впервые запускает настоящий SONARLOG одновременно с deterministic `SONARTST.EXE`, проверяет >128 записей/wrap, 10+ stop/restart циклов и запуск consumer-before-producer.
- `TESTLG60.TST` сохранён как regression алгоритма формата/PREAD/PWRITE.

Новых syscall нет. Формат SONAR.LOG не изменён.
