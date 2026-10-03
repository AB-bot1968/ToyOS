# Проверка FIX15 MTDATA

1. Собрать ToyOS штатным W64DevKit build.sh и проверить лимит kernel.bin 128 секторов.
2. Запустить SENSOR1..SENSOR4 и убедиться, что RTSTAT/RTDATA продолжают работать.
3. Выполнить `execmt MT01.EXE MT02.EXE MT03.EXE MT04.EXE`.
4. После завершения задач выполнить `mtlist`: завершённые задачи не должны выводиться.
5. Выполнить `mtdata MT1`, `mtdata MT2`, `mtdata MT3`, `mtdata MT4`. Ожидаются соответственно `MT01:DONE;` ... `MT04:DONE;` даже после SYS_EXIT.
6. До `mtstop all` повторный `execmt ...` должен быть отклонён как занятая MT-сессия.
7. Выполнить `mtstop all`. После этого `mtdata MT1` должен вывести `MTDATA: no data`.
8. Запустить новую EXECMT-сессию и проверить, что старые snapshots не доступны.
9. Проверить регистронезависимые варианты `MTDATA mt1`, `mtdata MT1`, `MtDaTa mT1`.
10. Повторить RTSTAT и RTDATA после теста MT, чтобы исключить регрессию RT.

Примечание: локально выполнена host-компиляция i386 freestanding. Полный W64DevKit/QEMU runtime требуется выполнить в целевой среде.
