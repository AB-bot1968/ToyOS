# v43 — восстановление help и добавление `ls` / SYS_LS

v43 является новой отправной точкой проекта после v41.

## Исправлено

1. В `help` снова присутствует команда:
   `ls`
2. В `help` снова присутствует команда:
   `execmt FILE1.EXE [FILE2.EXE ... FILE25.EXE]`
3. Команда `ls` реально подключена к shell и вызывает `SYS_LS=24` через `INT 80h`.
4. `SYS_LS` перечисляет регулярные файлы FAT16 root directory.
5. Имена выводятся **одной строкой**, разделяются одиночными пробелами и имеют VGA attribute `0x01` (blue).
6. Удалённые записи, LFN, volume label и directory entries не выводятся.
7. `SYS_LS` доступен обычным Ring-3 EXE1. `QPASS.EXE` теперь вызывает его и сообщает `QPASS: SYS_LS PASS/FAIL`.

## ABI

`SYS_LS = 24`:
- EBX = 0
- ECX = 0
- EDX = 0
- return `0` — список успешно выведен;
- return `0xffffffff` — FAT16/ATA error.

## Проверки

- `check30.sh` ... `check39.sh`: PASS
- `check43.sh`: PASS
- `sh -n build.sh`: PASS
- `sh -n check43.sh`: PASS
- kernel/user shell/QPASS i386 `-Wall -Wextra -Werror -fsyntax-only`: PASS
- host tools `mkfat16`, `fat16check`, `mkexe` с `-Wall -Wextra -Werror`: PASS

Полный PE/COFF build W64DevKit и запуск QEMU в текущей среде не выполнялись; их результат должен быть проверен в Windows 7 + W64DevKit.
