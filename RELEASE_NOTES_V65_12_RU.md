# Toy OS v65.12 — PNG2RAW

Базой является полностью проверенная v65.11.

## Новая возможность

В каталог `tools/` добавлена утилита для Windows 7:

```text
PNG2RAW.BAT input.png SPLASH.RAW
```

Она использует штатные Windows PowerShell + .NET `System.Drawing` и не требует
установки Python, Pillow, ImageMagick или сторонних DLL.

Результат:

```text
320x200
256 цветовых индексов VGA
64000 байт
```

Полученный `SPLASH.RAW` можно сразу заменить в `resources/`, после чего обычный
`build.bat` включит новый файл в FAT16.

## Примеры

```text
tools\PNG2RAW.BAT my_splash.png resources\SPLASH.RAW
build.bat
```

`AUTOSTART.SH` по-прежнему выполняет:

```text
exec VGADRV.EXE SPLASH.RAW
```

Остальная архитектура и поведение v65.11 не менялись.
