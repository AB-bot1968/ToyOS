# Toy OS v62 — протокол проверки

1. `sh -n build.sh` и `sh -n check68.sh`.
2. `path.c` — 32-bit freestanding compile.
3. `kernel.c` — 32-bit freestanding syntax check.
4. Host `pathcheck.c` — `-Wall -Wextra -Werror`.
5. Проверить `.` / `..` / повторные `/` / absolute / relative.
6. Проверить `path_join`, `path_parent`, `path_basename`.
7. Проверить отсутствие `fat_normalize_path` в kernel.
8. Проверить `build/path.o` в linker command.
9. Проверить kernel limit 128 sectors.
10. Запустить полный `build.sh` в W64DevKit x86.
11. Загрузить `build/toy_os.img` и проверить `pwd`, `cd`, `ls`, `cwd-test`.

Примечание: текущая Linux-проверка не является эмуляцией реального BIOS/Windows 7.
Поэтому успешный host/static тест не заменяет загрузку образа на целевой среде.
