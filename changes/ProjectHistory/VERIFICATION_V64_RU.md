# Toy OS v64 — верификация

## Базовая точка
Исходная точка: `ToyOS_v63_1_RMDIR_RELATIVE_FIX.zip` с SHA-256:
`5abd2361b921d211112cf152f4b857f29021e7249ea88bff6bad5852c1097b17`.

## Что проверено статически в Linux-контуре
- `src/kernel.c` компилируется как freestanding i386 (`gcc -m32`) без ошибок.
- `src/user_shell.c` компилируется как freestanding i386 (`gcc -m32`) без ошибок.
- `src/path.c` компилируется как freestanding i386 (`gcc -m32`) без ошибок.
- `tools/pathcheck.c + src/path.c` проходят hosted `-std=c99 -Wall -Wextra -Werror`.
- `build.sh`, `check74.sh`..`check77.sh` проходят shell syntax check.
- Все новые статические регрессии v64 проходят.

## Что должно быть выполнено на целевой Windows 7 + W64DevKit x86
`build.sh` намеренно требует i686-target. После полной сборки:
1. проверить `build/disk.img` через существующие `fat16check` и полный набор regression checks;
2. загрузить образ в QEMU;
3. проверить `help`, `help 2`, `help 3`, `help 4`;
4. выполнить runtime matrix `cp` ниже.

## Runtime matrix `cp`
```text
write A one
cp A B
cat B
filesize A
filesize B

mkdir D
cp A D
cat D/A

cd D
cp ../A C
cat C
cd ..

cp A A
cp ./A ./X/../A

mkdir E
cp E F
cp A E

write BIG ...
cp BIG COPY
cat COPY
```

Ожидаемое поведение:
- обычная копия успешна;
- копирование в существующий каталог использует basename SOURCE;
- относительные пути учитывают CWD;
- SOURCE=DEST после нормализации отклоняется;
- каталог как SOURCE отклоняется;
- существующий файл DEST может быть перезаписан содержимым SOURCE;
- при ошибке операции старый DEST не должен быть предварительно уничтожен.
