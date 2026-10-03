# Проверка ToyOS v67.11-B FIX45

1. Сначала: `test TESTPOL.TST`. Ожидается FAIL=0 PASSED.
2. Затем: `test TESTSUP.TST` и `test TESTWD.TST`; оба должны проверить SAFE после исчерпания рестартов и вернуть NORMAL перед выходом.
3. Полная регрессия: TESTLOAD, TESTKEY, TESTEVT, TESTLOG, TESTBOOT, TESTSAFE, TESTPOL, TESTHB, TESTWD, TESTSUP, TESTRES, TESTPROC, TESTCORE.
4. После регрессии без ручной очистки выполнить `execmt MT01.EXE`; session active быть не должно.
5. Ручная проверка: `safemode normal 0`, затем `supstart FAULTUD.EXE`, дождаться завершения supervisor, `safemode` должен показать SAFE reason=4591. После проверки `mtstop all` и `safemode normal 0`.
