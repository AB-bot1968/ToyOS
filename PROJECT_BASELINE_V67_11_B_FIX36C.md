# ToyOS v67.11-B FIX36C TEST INFRASTRUCTURE

Основа: runtime-confirmed stable FIX36B.

FIX36C добавляет минимальную инфраструктуру автоматизированных regression scripts в Ring3 shell. Scheduler, RT/MT policy, process lifecycle и syscall ABI 0..56 не изменены.

Новая команда: `test FILE.TST`.

Формат .TST намеренно не является полноценным shell-языком:
- `# ...` — комментарий;
- `PRINT text` — диагностическое сообщение;
- `RUN command` — выполнить обычную команду ToyOS через существующий case-insensitive dispatcher;
- `ASSERT RT_ACTIVE N` — число живых RT процессов равно N;
- `ASSERT MT_ACTIVE N` — число живых MT процессов равно N;
- `ASSERT PID_ACTIVE PID` — PID находится READY/RUNNING/BLOCKED;
- `ASSERT PID_EXITED PID` — PID находится EXITED;
- `ASSERT PID_STATE PID STATE` — точная проверка process state (1 READY, 2 RUNNING, 3 BLOCKED, 4/5 EXITED согласно PROCESS_INFO).

ASSERT использует SYS_PROCESS_INFO=55 и не разбирает текст `ps`. RUN сам по себе не считается проверкой успешности команды: проверяемое условие должно быть выражено отдельным ASSERT.

В FAT16 образ добавлены TESTCORE.TST и TESTPROC.TST. TESTCORE рассчитан на чистый старт без RT/MT задач.

FIX36B сохраняется как runtime-confirmed stable rollback. FIX36C становится stable только после W64DevKit/QEMU runtime verification.
