# Проверка FIX13

1. Запустить SENSOR1..SENSOR4 через RTD/F10. Проверить `rtstat` и `rtdata SLOT1..SLOT4`.
2. Выполнить `execmt MT01.EXE MT02.EXE MT03.EXE MT04.EXE`. Команда должна сразу вернуть shell.
3. `mtstat` должен показать MT1..MT4; RT SLOT1..SLOT4 должны продолжать работу и RTDATA D2 должен расти.
4. Проверить `mtstop MT2`; MT2 исчезает из активной очереди, остальные продолжают round-robin.
5. Проверить `mtstop ALL`; MT прекращается, RT задачи остаются активными.
6. Повторить `rtstat watch` во время EXECMT; контролировать MISS/SKIP.
7. Проверить, что `rt-jitter` и `mt-test` больше не являются shell-командами.
8. Проверить самостоятельный `SYS_EXIT` каждой MT-задачи и возврат к shell после последней.
