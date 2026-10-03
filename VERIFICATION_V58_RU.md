# v58 — исправление команды `exec LOADER.EXE`

## Симптом

Команда:

    exec LOADER.EXE 1 RECV FF.EXE

возвращала:

    usage: LOADER: usage PORT RECV FILE.EXE

## Причина

В v57 shell имел специальный обработчик для `exec COMDRV.EXE`, который
упаковывал три аргумента в существующий ABI `SYS_EXEC_ARGS`.

Для `LOADER.EXE` такого обработчика не было. Команда попадала в общий
обработчик `exec`, который передаёт только имя EXE1 без аргументного блока.
Поэтому LOADER действительно запускался, но EBX/ECX/EDX не содержали
`PORT`, `RECV` и имени файла.

## Исправление

В `src/user_shell.c` добавлен только специальный пользовательский helper
`exec_loader_command()`.

Он:

1. получает строку после `exec LOADER.EXE `;
2. разбирает `PORT`, `RECV`, `FILE.EXE`;
3. проверяет отсутствие лишнего текста;
4. упаковывает три строки в существующие 48 байт ABI;
5. вызывает существующий `SYS_EXEC_ARGS` для `LOADER.EXE`.

В `shell_run()` специальная проверка расположена перед общим `exec`, поэтому
`exec LOADER.EXE ...` больше не попадает в обработчик `exec_file()`.

## Изменение ядра

В v58 `src/kernel.c` не изменялся.

Существующий v57 механизм передачи трёх аргументов EXE1 остаётся прежним:

- EBX -> PORT;
- ECX -> DIRECTION;
- EDX -> FILE.

## Команда проверки

    exec LOADER.EXE 1 RECV FF.EXE

Ожидается, что LOADER больше не выдаёт `usage` и выводит:

    LOADER: receiving EXE1 and starting it

После этого очередь v57 должна запустить:

    COMDRV.EXE 1 RECV FF.EXE

а после успешного `SYS_EXIT(0)` COMDRV — `FF.EXE`.

## Статическая проверка

`check58.sh` проверяет наличие специального обработчика, точного префикса,
смещения `line+16`, вызова `SYS_EXEC_ARGS` и строки справки.

Дополнительно `gcc -m32 -ffreestanding ... -fsyntax-only src/user_shell.c`
проходит синтаксическую проверку. Имеющееся предупреждение
`-Wmisleading-indentation` относится к неизменённой строке v57
`exec_queue_com_command()` и не связано с исправлением v58.
