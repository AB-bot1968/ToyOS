# Проверка ToyOS v67.11-B FIX36C TEST INFRASTRUCTURE

## 1. Сборка
Собрать штатным `build.sh` в 32-bit W64DevKit. Сборка обязана пройти target check i686, FAT16 check для TESTCORE.TST/TESTPROC.TST и все прежние check scripts.

## 2. Контроль FIX36B
До тестирования новой команды повторить основные проверки FIX36B: shell, RT, MT, RT+MT, `wait PID` для завершившегося процесса и отмену ожидания активного RT/MT через ESC. Поведение не должно отличаться от stable FIX36B.

## 3. Базовый автоматический тест
После чистой загрузки, до запуска RT/MT:
`test TESTCORE.TST`
Ожидается выполнение pit-test и ps, затем PASS для `RT_ACTIVE 0` и `MT_ACTIVE 0`, итог `FAIL=0 PASSED`.

Проверить регистронезависимость:
`TeSt testcore.tst`
Результат должен быть тем же.

## 4. Проверка ASSERT на реальном состоянии
Запустить:
`execmt MT01.EXE MT02.EXE MT03.EXE MT04.EXE`
Создать файл через shell:
`write MTTEST.TST ASSERT MT_ACTIVE 4`
Выполнить:
`test MTTEST.TST`
Ожидается PASS/FAIL=0.
Затем `mtstop MT4` и снова `test MTTEST.TST`: теперь ASSERT обязан дать FAIL. Это намеренная отрицательная проверка test runner, а не ошибка ОС.
После проверки: `mtstop all`.

## 5. PID assertions
По `ps` выбрать реально существующий активный PID N. Создать `write PIDTEST.TST ASSERT PID_ACTIVE N` и выполнить `test PIDTEST.TST`: PASS. После остановки/выхода процесса `ASSERT PID_ACTIVE N` должен FAIL, а `ASSERT PID_EXITED N` должен PASS, пока metadata PID сохранена.

## 6. Ошибки сценария
Проверить несуществующий файл: `test NONE.TST` -> `TEST: file not found` и нормальный возврат prompt.
Создать файл с неизвестной директивой: `write BAD.TST BOGUS` и `test BAD.TST` -> FAIL, без зависания ОС.

## 7. Повторная RT/MT regression
После тестов повторить 4RT+4MT, RTSTAT/MTSTAT/PS и FIX36B wait/ESC сценарий. Test infrastructure не должна менять scheduler statistics или scheduling semantics.

## Критерий stable
FIX36C фиксировать stable только если W64DevKit build проходит и все пункты выше проходят в QEMU. При любой scheduler/process regression откат — FIX36B.
