# Verification ToyOS v67.11-B FIX59

Статус релиза при выпуске архива: CANDIDATE до runtime-прогона пользователем в W64DevKit/QEMU.

Проверить на Windows 7/W64DevKit: `tools\\sonarsim\\build_w64devkit.bat`; ожидается `SONARSIM SELFTEST: PASS`.
В ToyOS: `test TESTSIM.TST`, затем `TESTSON.TST`, `TESTMB.TST`, `TESTDATA.TST`, `TESTUART.TST`, `TESTINF.TST` и полный regression suite; везде FAIL=0.

Для реального E2E связать QEMU guest COM1 с Windows COM/virtual serial pair, запустить SONARSIM normal/fixed XYZ, затем SONARDRV под supervisor. Fault modes предназначены для последующей/ручной E2E проверки поведения timeout/retry/recovery.

Architecture Vision изменён: добавлен внешний host-side deterministic simulator; mission, kernel ABI, I/O-port policy, scheduler и recovery ABI не изменены.
