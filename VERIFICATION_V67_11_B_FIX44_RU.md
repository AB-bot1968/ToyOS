# Проверка FIX44

1. Собрать build.sh в W64DevKit и загрузить ToyOS в QEMU.
2. Выполнить `test TESTSAFE.TST`. Ожидается FAIL=0/PASSED.
3. Ручная проверка: `safemode`, `safemode degraded 123`, `safemode`, `safemode safe 456`, `safemode`, `safemode normal 0`.
4. Полная регрессия: TESTLOAD, TESTKEY, TESTEVT, TESTLOG, TESTBOOT, TESTSAFE, TESTHB, TESTWD, TESTSUP, TESTRES, TESTPROC, TESTCORE. Все FAIL=0.
5. После регрессии без ручной очистки выполнить `execmt MT01.EXE`; session active быть не должно. Затем `mtstop all`.

FIX44 не меняет scheduler policy: SAFE/DEGRADED пока являются диагностическим состоянием/фундаментом будущей policy.
