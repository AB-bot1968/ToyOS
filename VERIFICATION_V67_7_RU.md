# ToyOS v67.7 — verification

## Static verification

`./check133.sh`

Результат:

`CHECK133 PASS: file deletion fully removes FAT16 directory entries and stale duplicates`

## Compilation

`src/kernel.c` успешно компилируется с:

- `-m32`
- `-ffreestanding`
- `-fno-builtin`
- `-fno-stack-protector`
- `-nostdinc`
- `-I src`

## Runtime regression scenario

Ожидаемое поведение:

1. создать `010101.TXT`;
2. выполнить `ls` — имя присутствует;
3. выполнить `delete 010101.TXT` или `rm 010101.TXT` — `OK`;
4. выполнить `ls` — `010101.TXT` отсутствует;
5. повторно создать файл с тем же именем — имя появляется только один раз;
6. повторно удалить его — имя снова отсутствует.

Полная `build.sh` не заявляется проверенной в среде без проектного i686/W64DevKit toolchain.
