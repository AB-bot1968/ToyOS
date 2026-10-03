# Toy OS v65.4 — исправление передачи аргумента VGADRV.EXE

## Исправленная ошибка

Команда:

```text
exec VGADRV.EXE SPLASH.RAW
```

могла приводить к сообщению:

```text
VGADRV: usage exec VGADRV.EXE FILE
```

Причина находилась не в FAT16 и не в разборе команды shell. `VGADRV.EXE`
получал аргументы через `EBX/ECX/EDX`, но C-функция читала `EDX` пустым
inline-asm с output-операндом. Такой код не описывал для компилятора входное
значение регистра и не являлся корректным C ABI-контрактом.

## Исправление

Точка входа VGADRV теперь использует обычный cdecl-мост:

```text
_start:
    push EDX
    push ECX
    push EBX
    call program_main
```

C-функция имеет явную сигнатуру:

```c
void program_main(const char *port,
                  const char *dir,
                  const char *file);
```

Для VGADRV используется третий параметр `file`. Первые два параметра
принимаются явно и не используются.

Остальная архитектура v65 не меняется: `SPLASH.RAW` остаётся обычным файлом
FAT16, `VGADRV.EXE` работает в Ring 3, VGA-порты доступны через системные
вызовы, VRAM отображается отдельным syscall.
