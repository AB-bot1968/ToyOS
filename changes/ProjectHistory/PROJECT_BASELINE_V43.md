# Project baseline — v43

С этого состояния v43 считается новой отправной точкой проекта Toy OS.

Базовый архив: `toy_os_w7_shell_ring3_fixed_ru_v43_help_ls.zip`.

Сохранены все требования и функциональность предыдущей ветки, включая:
- GNU as bootloader, real mode -> protected mode;
- flat 1 GiB GDT code/data segments;
- kernel at physical 0x7E00;
- W64DevKit/Windows 7 target;
- PE/COFF i386 linking через `ld -m i386pe`;
- `liker.ld`, `objcopy`, kernel 64-sector guard;
- FAT16 root 8.3;
- legacy `exec`, `execq`, EXE1;
- scheduler lifecycle and CR3 isolation;
- `EXECMT_MAX_TASKS=25`, `SYS_EXECMT=23`;
- `SYS_LS=24`, shell `ls`, and EXE1 access to the same syscall;
- `ls` output as one space-separated blue VGA line;
- `execmt` and `ls` are mandatory entries in `help`.
