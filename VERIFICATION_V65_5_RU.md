# Проверка Toy OS v65.5

## Основная исправленная цепочка

```text
exec VGADRV.EXE SPLASH.RAW
        ↓
SYS_EXEC_ARGS
        ↓
ядро сохраняет аргумент №2
        ↓
VGADRV → SYS_EXEC_ARG(2, buffer, 16)
        ↓
open SPLASH.RAW
        ↓
SYS_VIDEO_MAP → VGA → VRAM
```

Проверено статически и компиляцией: `VGADRV` больше не извлекает имя файла
из стартовых регистров; syscall 34 имеет проверки Ring 3, индекса и user-буфера.

Полный запуск Windows 7/W64DevKit/QEMU в текущей Linux-среде не выполнялся.
