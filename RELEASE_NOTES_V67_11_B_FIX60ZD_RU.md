# ToyOS v67.11-B FIX60ZD TEST SCOPE RT FIX

- TESTKEY.TST и TESTACC.TST сохранены: оба тестируют уникальные необходимые функции.
- START RT теперь использует минимальный SENSOR.EXE, чтобы acceptance scheduler/STOP не зависел от диагностических RT_EXEC_INFO/RT_DATA SENSOR1..8.
- TESTKEY keyboard/F10 RT workload также использует SENSOR.EXE 100/100 ms.
- Диагностические SENSOR1..8 не удалены и остаются для специализированной телеметрии.
- TST.LOG/B*.TST не возвращены.
