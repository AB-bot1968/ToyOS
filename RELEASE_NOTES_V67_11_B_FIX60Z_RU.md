# ToyOS v67.11-B FIX60Z

Исправления после реального полного прогона FIX60Y:

- TESTLGR уже проходит и его рабочий код не изменён.
- MT_HEARTBEAT/MT_HEARTBEAT_WAIT теперь выбирают PID только активной MT-задачи (READY/RUNNING/BLOCKED).
- SENSOR1..SENSOR8 больше не завершаются из-за временного отказа диагностических RT_EXEC_INFO/RT_DATA; границей RT job остаётся RT_WAIT.
- 43 исходных TEST*.TST не изменены.
- TST.LOG и B*.TST отсутствуют; итоговая консольная сводка сохранена.
