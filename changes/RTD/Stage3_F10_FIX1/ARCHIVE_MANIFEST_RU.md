# Состав архива ToyOS v67 RTD Stage3 F10 FIX1

База: `ToyOS v67 RTD Stage3 F10`.

Исправленные технические файлы:

- `src/user_shell.c` — mutable F10/RTD shell state → `.userdata`;
- `src/kernel.c` — границы user-data и RW+US mapping;
- `liker.ld` — новая `.udata` section;
- `build.sh` — включение `.udata` в `kernel.bin`;
- `check_rtd_stage3_f10.sh` — regression assertions;
- `check_vga_text_shell_userdata.sh` — новый focused test.

Документация текущего FIX1 находится в `changes/RTD/Stage3_F10_FIX1/`.
Исторические Markdown-файлы зеркалируются в `changes/ProjectHistory/`.
