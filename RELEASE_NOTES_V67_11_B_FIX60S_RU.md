# ToyOS v67.11-B FIX60S — SONARLOG deterministic endurance

FIX60S исправляет недостатки приёмки FIX60R, не изменяя принятый рабочий тракт
SONARDRV/UART/QEMU TCP/SONARSIM.

- Добавлен SONARPUB.EXE — минимальный paced producer Data Channel 0 без UART/Modbus/FAT.
- TESTLGR.TST переработан: реальный SONARLOG, 128-record wrap, CRC последней записи,
  9 stop/restart циклов, consumer-first startup и финальная интеграция с SONARTST.
- Сохранены исправления SONARLOG: wait/reacquire producer, recovery файла 4160 bytes,
  heartbeat и PID-owned Data Channel reader cleanup.
- Добавлен ASSERT SONAR_LOG_VALID N.
- SONARDRV, UARTRX и SONARSIM byte-identical принятому FIX60Q STABLE.
