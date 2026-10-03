# VERIFICATION v48 — EXECMT user-segment fix

## Исправленная ошибка

В v47 scheduler создавал начальный Ring-3 контекст для EXECMT, но слова
GS/FS/ES/DS оставались равны нулю. При `iret` процесс действительно входил
в Ring 3, однако первая загрузка данных из `task_line` выполнялась через
недействительный DS=0 и могла приводить к #GP до `SYS_CONSOLE_WRITE`.

## Исправление

В `sched_load_initial()` начальные пользовательские сегменты явно установлены:

- GS = 0x23
- FS = 0x23
- ES = 0x23
- DS = 0x23
- CS = 0x1B
- SS = 0x23
- EFLAGS = 0x202

Стек и CR3 EXECMT не изменены.

## Сохранено

- `exec`
- `execq`
- `execmt`
- `ls`
- меню shell и существующие команды
- отдельные CR3 и физические image slots EXECMT
- отдельные физические stack pages
- диагностическая строка `[MTxx] RUNNING ...`
- однократный `SYS_CONSOLE_WRITE` строки `Task NN\\n`
- SYS_EXIT / scheduler lifecycle

## Проверка

`check48.sh` проверяет наличие исправления и сохранение ключевого EXECMT ABI.

Полная штатная сборка W64DevKit/QEMU в текущем Linux-окружении не выполняется,
поскольку проект требует i686 W64DevKit; исходники проверены статически.
