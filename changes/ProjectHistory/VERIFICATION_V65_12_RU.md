# Проверка Toy OS v65.12

## Проверенные изменения

- `tools/PNG2RAW.BAT` существует.
- `tools/PNG2RAW.ps1` существует.
- `tools/PNG2RAW_RU.md` существует.
- Скрипт использует штатный `System.Drawing.Bitmap`.
- Целевой размер — `320x200`.
- Выходной размер — ровно `64000` байт.
- Палитра соответствует индексации `VGADRV`: 216 цветов RGB cube + 40 grayscale.
- `check99.sh` проходит.

## Проверка пользователя Windows 7

На Windows 7 с PowerShell необходимо выполнить:

```text
tools\\PNG2RAW.BAT my_splash.png resources\\SPLASH.RAW
```

Ожидается:

```text
PNG2RAW: OK
Format: 320x200, 256-color VGA RAW, 64000 bytes
```

Затем:

```text
build.bat
```

И `AUTOSTART.SH` продолжит автоматически выполнять `exec VGADRV.EXE SPLASH.RAW`.

Полный runtime-тест Windows 7 в Linux-среде разработки не выполнялся.
