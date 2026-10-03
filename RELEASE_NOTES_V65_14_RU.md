# Toy OS v65.14 — PNG2RAW на GCC

На базе рабочей v65.11/v65.13 добавлен полноценный C-конвертер PNG -> RAW:

```text
tools/png2raw.c
```

Он собирается GCC из W64DevKit x86 автоматически при каждой сборке проекта:

```text
tools/png2raw.c -> build/png2raw.exe -> tools/PNG2RAW.EXE
```

Для распаковки PNG используется zlib (`-lz`). Утилита статически линкуется
по GCC runtime, чтобы запуск через BAT не зависел от отдельного MinGW runtime.

`tools/PNG2RAW.BAT` теперь запускает именно `tools/PNG2RAW.EXE`.

PowerShell-версия `PNG2RAW.ps1` сохранена как исторический/резервный материал,
но стандартный BAT её больше не запускает.
