# Проверка ToyOS v67.11-B FIX36D

## 1. Сборка
Собрать штатным build.sh в W64DevKit i386. Ошибок компиляции/линковки быть не должно.

## 2. Критический regression #PF FIX36C
После чистой загрузки выполнить:

    test TESTCORE.TST

Не должно быть EXC 14. Ожидается завершение TEST RESULT с FAIL=0 PASSED.
Затем сразу проверить `ps`, `ls`, `rtstat`, `mtstat`. Консоль должна отвечать.

## 3. Отрицательный ASSERT
    execmt MT01.EXE MT02.EXE MT03.EXE MT04.EXE
    write MTTEST.TST ASSERT MT_ACTIVE 4
    test MTTEST.TST

Ожидается PASS. Затем:

    mtstop MT4
    test MTTEST.TST

Ожидается FAIL (это намеренная отрицательная проверка runner). После нее shell должен работать.

## 4. RUN и глубина стека
Создать тесты с `RUN ps`, `RUN rtstat`, `RUN mtstat`, `RUN ls` по отдельности и вместе. Ни один не должен давать #PF. `RUN test TESTCORE.TST` должен быть отвергнут как nested test и не зависать.

## 5. FIX36B regression WAIT
Повторить подтвержденный сценарий 4 RT + 4 MT. `wait PID` активного MT и активного RT, затем ESC. Shell должен вернуться, ожидаемая задача должна продолжить работу. После остановки задачи `wait PID` должен вернуть сохраненный результат.

## 6. Scheduler regression
При 4RT+4MT проверить RTSTAT/MTSTAT и WATCH/ESC. Счетчики должны изменяться; запуск/остановка задач и консоль не должны зависать. Проверить fault diagnostics из FIX35B.

## 7. Статус
FIX36D считать stable только после полного runtime теста. FIX36B остается контрольной stable версией.
