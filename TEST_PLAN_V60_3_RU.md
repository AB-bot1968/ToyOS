# План тестирования Toy OS v60.3

## 1. Сборка

В Windows 7 откройте **x86 W64DevKit** и выполните:

```text
cd путь\к\toy
build.bat
```

Ожидается отсутствие `No such file or directory` после `mkexe.exe`.
Сборка должна завершиться строкой:

```text
Build and verification OK.
```

## 2. Контроль EXE1

В `build` должны существовать непустые файлы:

```text
HELLO.EXE
COMDRV.EXE
LOADER.EXE
NETDRV.EXE
QPASS.EXE
MT01.EXE
```

## 3. FAT16 ROOT

В QEMU:

```text
ls
cat /NET.CFG
exec /HELLO.EXE
```

## 4. Каталоги

```text
ls /BIN
ls /DOC
exec /BIN/HELLO.EXE
cat /DOC/NET.CFG
```

## 5. Новые проверки

Скрипт `check63.sh` автоматически проверяет три уровня:

1. исходный код и интеграцию `make_exe1`;
2. три варианта компиляции `mkexe.c`;
3. реальные EXE1-файлы и их записи в FAT16.
