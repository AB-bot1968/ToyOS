# Toy OS v60.4 — обязательная проверка сборки на Windows 7

## Среда

- Windows 7
- W64DevKit x86 (32-bit)
- QEMU для запуска образа

## Сборка

Открыть W64DevKit Shell в каталоге проекта и выполнить:

```sh
build.bat
```

Критическая контрольная точка v60.4:

```text
[EXECMT-TEST] compiling task 25
[EXE1] build/mt25.raw -> build/MT25.EXE (BSS=0)
FAT16 image created: build/disk.img, ...
[FAT16] disk.img created: ... bytes
```

Если `MT25.EXE` создан, но `disk.img` отсутствует, `build.sh` теперь выводит
явную ошибку и завершает работу.

## Проверка root

После сборки проверить:

```sh
build/fat16check.exe build/disk.img HELLO.EXE
build/fat16check.exe build/disk.img MT25.EXE
build/fat16check.exe build/disk.img NETDRV.EXE
build/fat16check.exe build/disk.img NET.CFG
build/fat16check.exe build/disk.img BIN/HELLO.EXE
build/fat16check.exe build/disk.img DOC/NET.CFG
```

## QEMU

Запустить полученный `build/toy_os.img` и проверить:

```text
ls
ls /BIN
ls /DOC
cat /NET.CFG
```
