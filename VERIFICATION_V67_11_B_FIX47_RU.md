# Проверка FIX47

Сначала: `test TESTHLTH.TST`. Ожидается FAIL=0. Затем выполнить полный набор regression TST от FIX46 плюс TESTHLTH.TST. После полного набора без ручной очистки проверить `execmt MT01.EXE`, `health`, `safemode`, `hwwd`. Health должен быть NORMAL, handles=0 до запуска MT, watchdog disarmed.
