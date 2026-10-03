# Проверка FIX60ZE

- База: зафиксированный FIX60ZHD.
- `src/kernel.c` не изменён относительно FIX60ZHD (байтовое сравнение).
- Изменён `src/user_shell.c`: `exec SONARVWR.EXE` использует временный изолированный MT process через существующие SYS_PROCESS_SPAWN/SYS_PROCESS_WAIT; остальные `exec` используют прежний SYS_EXEC.
- `TESTVWR.TST` усилен проверкой heartbeat SONARLOG во время асинхронно работающего SONARVWR.
- ACCEPT.TXT: 45 уникальных тестов, все файлы существуют.
- TESTVWR присутствует в FAT payload и post-build manifest build.sh.
- Максимальная длина строки TST: 106, лимит интерпретатора 400.
- `kernel.c`, `user_shell.c`, `sonarview.c` успешно компилируются доступным host GCC в режиме i386 freestanding.
- `bash -n build.sh` проходит.
- Полная штатная сборка в данной среде не выполняется: build.sh требует x86 W64DevKit с target i686; доступный gcc не является требуемым W64DevKit. Это ограничение среды, а не заявленный runtime PASS.
- В дереве релиза отсутствуют build/, TST.LOG и B*.TST.

Финальная runtime-проверка выполняется на целевой среде пользователя командой `test` и сценарием SONARDRV/SONARLOG + RT + повторный `exec SONARVWR.EXE`.
