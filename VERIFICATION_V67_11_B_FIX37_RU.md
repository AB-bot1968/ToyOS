# Проверка FIX37

FIX36E — стабильная точка отката. FIX37 до runtime-проверки является кандидатом.

1. Собрать штатным `build.sh` в W64DevKit и загрузить ToyOS.
2. `help`, `help 2`, `help 3`: убедиться, что вывод компактный, читаемый и не уходит за экран.
3. `spawn ARGVDIAG.EXE ONE TWO THREE`. Команда должна сразу вернуть `spawn: PID=N`; ARGVDIAG публикует строку argv через MTDATA (не пишет в общую консоль) и завершается как MT-class процесс. Найдите его MT-слот через `ps`, проверьте `mtdata MTn`, затем `wait N`: нормальное завершение status 0.
4. Повторить несколько spawn подряд, проверить уникальные PID.
5. Запустить 4 MT через execmt, затем spawn ARGVDIAG.EXE A B; проверить mtstat/ps и отсутствие зависания.
6. Запустить 4 RT + 4 MT и повторить spawn/wait. RTSTAT MISS/SKIP не должны начать расти из-за изменения политики.
7. Регрессии FIX36E: TESTCORE.TST, TESTPROC.TST, CRLF, wait активного PID + ESC, RTSTAT WATCH ESC, MTSTAT WATCH ESC.
8. Проверить mtstop ALL после spawn/execmt.

Важно: assistant выполнил только host/static проверки; полная W64DevKit/QEMU runtime-проверка требуется на вашей среде.
