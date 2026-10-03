# Проверка Toy OS v65.10

## Изменённые компоненты

- `src/kernel.c` — исправлен доступ к font plane 2 и финальная текстовая адресация VGA.
- `check87.sh`, `check89.sh`, `check91.sh`, `check92.sh` — обновлены согласно канонической конфигурации text mode.
- `check93.sh` — новый регрессионный тест точных параметров font access и порядка восстановления.

## Результаты

`check78.sh`–`check93.sh`: PASS.

32-bit freestanding compilation:

- `src/kernel.c`: PASS с проектными флагами; присутствует историческое `-Wunused-function` для `fat_find_free_dir`.
- `src/user_shell.c`: PASS с проектными флагами; присутствует историческое `-Wunused-function` для `sys_ls`.
- `src/vgadrv.c`: PASS без предупреждений.
- остальные пользовательские C-модули: PASS.

Host tools:

- `tools/mkfat16.c`: `-Wall -Wextra -Werror` PASS.
- `tools/mkexe.c`: `-Wall -Wextra -Werror` PASS.
- `tools/fat16check.c`: `-Wall -Wextra -Werror` PASS.
- `tools/pathcheck.c + src/path.c`: `-Wall -Wextra -Werror` PASS.

Runtime на Windows 7 + W64DevKit x86 + QEMU в текущей среде не выполнялся, поэтому этот результат здесь не заявляется.
