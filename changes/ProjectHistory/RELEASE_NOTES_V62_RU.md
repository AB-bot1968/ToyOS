# Toy OS v62 — модуль путей

## Цель

После стабилизации v61.2 обработка путей вынесена из `kernel.c` в отдельный
модуль `src/path.c` с интерфейсом `src/path.h`. Это уменьшает дублирование и
создаёт безопасную основу для дальнейшего расширения shell и файловой системы.

## Реализовано

- `path_is_absolute()`;
- `path_normalize()` — абсолютные/относительные пути, `.`, `..`, повторные `/`;
- приведение имён FAT 8.3 к верхнему регистру;
- `path_join()`;
- `path_parent()`;
- `path_basename()`;
- запрет LFN сохраняется намеренно;
- `kernel.c` использует единый `path_normalize()` вместо собственного нормализатора;
- `path.c` собирается как отдельный 32-битный freestanding object и линкуется в kernel;
- добавлен host regression `tools/pathcheck.c`.

## Совместимость

FAT16 layout v61.2 не меняется:

- kernel slot — 128 секторов;
- scratch — LBA 129;
- FAT16 — LBA 130;
- ROOT — 512 записей;
- каталоги — обычные FAT16 cluster chains.

## Проверка

Сборка должна выполнить `tools/pathcheck` и вывести:

`PASS: v62 path module regression`

После этого выполняются стандартные FAT16, EXE1, Ring-3, cwd/cd/pwd и queue/MT
регрессии. На целевой Windows 7 + W64DevKit обязательно выполнить полный `build.bat`.
