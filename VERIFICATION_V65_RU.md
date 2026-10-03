# Проверка Toy OS v65

## Выполнено в текущей среде

1. `src/kernel.c` собран как freestanding i386 C через host GCC `-m32`.
2. `src/user_shell.c` собран как freestanding i386 C через host GCC `-m32`.
3. `src/path.c` собран как freestanding i386 C через host GCC `-m32`.
4. `src/vgadrv.c` собран как freestanding i386 C через host GCC `-m32`.
5. `tools/mkfat16.c` собран со строгими `-O2 -std=c99 -Wall -Wextra -Werror`.
6. `tools/pathcheck.c + src/path.c` проходят строгую hosted-проверку.
7. `mkfat16` независимо создал FAT16 image с `SPLASH.RAW`; `fat16check`
   подтвердил `size=64000`, `first_cluster=3`, `clusters=125`.
8. Регрессии `check68.sh` ... `check78.sh` проходят.
9. `build.sh` и `check78.sh` прошли `sh -n`.

## Что требуется на Windows 7 / W64DevKit x86

Запустить:

```text
build.sh
```

Затем загрузить `build/toy_os.img` в QEMU.

Ручной графический тест:

```text
ls
filesize SPLASH.RAW
exec VGADRV.EXE SPLASH.RAW
```

Ожидается:

- FAT16 содержит `VGADRV.EXE` и `SPLASH.RAW`;
- драйвер запускается как Ring 3;
- VGA переключается в 320x200x256;
- 64000 байт изображения выводятся в VRAM;
- любая клавиша возвращает текстовый 80x25;
- `VGADRV: OK` и следующий prompt появляются в shell;
- после завершения драйвера user-доступ к VRAM закрыт.
