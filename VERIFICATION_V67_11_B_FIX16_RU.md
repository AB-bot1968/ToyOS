# Проверка FIX16

1. Запустить несколько/все MT01.EXE..MT25.EXE через EXECMT.
2. Убедиться, что сами фоновые MT-задачи ничего не печатают в общей консоли и не повреждают строку shell.
3. Проверить `MTDATA MT1`, `MTDATA MT2`, ...: результаты должны находиться только в соответствующих snapshot MTDATA.
4. Для MT02/MT03 проверить BLOCK/WAKE: конечные результаты должны быть доступны через MTDATA, а не через консоль.
5. После SYS_EXIT задача исчезает из MTLIST, но последний MTDATA остаётся доступен до MTSTOP ALL.
6. MTSTOP ALL очищает данные сессии согласно FIX15.
7. Параллельно запустить SENSOR1..SENSOR4 и проверить RTSTAT/RTDATA на отсутствие регрессии.

Host static compile of execmt_task.c for representative IDs 1,2,3,9,10,25 with i386 freestanding flags passed. Full W64DevKit/QEMU validation must be performed in the target environment.
