# Project baseline — v44

v44 продолжает v43 и исправляет совместимость `SYS_SCHED_EXIT` с обычным `exec`/`execq`, а также нормализует вывод `MTxx.EXE`.

База: `toy_os_w7_shell_ring3_fixed_ru_v43_help_ls.zip`.

Ключевые свойства:
- W64DevKit x86 / i386 freestanding kernel;
- bootloader GNU `as`;
- PE/COFF link через `ld -m i386pe`;
- raw binary через `objcopy`;
- `liker.ld`;
- EXECMT до 25 EXE1;
- `SYS_LS=24` и shell `ls`;
- обычные `exec` и `execq` остаются рабочими;
- `SYS_SCHED_EXIT` имеет fallback в legacy `exe_exit()` вне EXECMT;
- 10-pass verification chain сохраняется.
