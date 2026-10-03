# Набор EXE1 для проверки очередей

В проект добавлены четыре дополнительных standalone-программы EXE1. Они собираются автоматически и помещаются в FAT16-образ на каждом полном запуске `build.sh`.

## Файлы

- `QPASS.EXE` — обычный элемент очереди; выводит сообщение и завершает работу через `SYS_EXIT`.
- `QPORT.EXE` — элемент очереди, проверяющий `SYS_PORT_IN8` и `SYS_PORT_OUT8` из Ring 3.
- `QFROMEXE.EXE` — запускает `SYS_EXEC_QUEUE` непосредственно из Ring 3. Таблица имени находится в read-only `.urodata`, поэтому одновременно проверяется исправленная проверка `user_range`.
- `QSTOP.EXE` — вызывает `SYS_QUEUE_STOP` из активного EXE и должен немедленно вернуть управление сохранённой shell frame.

Все программы используют один и тот же `program.ld` и формат EXE1. Они выполняются последовательно в одном пользовательском слоте `0x00100000`, поэтому параллельного выполнения EXE1 нет.

## Автоматическая сборка

`build.sh`:

1. компилирует каждый исходник `src/queue_tests/*.c` как 32-битный freestanding Ring-3 объект;
2. линкует его по `program.ld`;
3. извлекает raw image;
4. упаковывает raw image утилитой `mkexe` в EXE1;
5. передаёт все EXE1 в `mkfat16`;
6. проверяет каждую запись `HELLO.EXE`, `QPASS.EXE`, `QPORT.EXE`, `QFROMEXE.EXE`, `QSTOP.EXE` через `fat16check`.

`mkfat16` теперь принимает несколько host-файлов и создаёт отдельные FAT16 root entries и цепочки кластеров для каждого файла.

## Рекомендуемые проверки из shell

После загрузки образа:

```text
execq 2 QPASS.EXE
execq 2 QPASS.EXE QPORT.EXE
execq 3 HELLO.EXE QPASS.EXE QPORT.EXE
execq 1 QSTOP.EXE QPASS.EXE
exec QFROMEXE.EXE
```

Для проверки максимального числа элементов можно использовать один и тот же EXE 25 раз:

```text
execq 1 QPASS.EXE QPASS.EXE QPASS.EXE QPASS.EXE QPASS.EXE QPASS.EXE QPASS.EXE QPASS.EXE QPASS.EXE QPASS.EXE QPASS.EXE QPASS.EXE QPASS.EXE QPASS.EXE QPASS.EXE QPASS.EXE QPASS.EXE QPASS.EXE QPASS.EXE QPASS.EXE QPASS.EXE QPASS.EXE QPASS.EXE QPASS.EXE QPASS.EXE
```

Для проверки большого конечного числа циклов:

```text
execq 3000 QPASS.EXE
```

Для бесконечного режима:

```text
execq forever QPASS.EXE QPORT.EXE
```

Остановить активную бесконечную очередь с клавиатуры:

```text
Esc
```

Надёжная проверка остановки непосредственно из EXE:

```text
execq forever QSTOP.EXE QPASS.EXE
```

`QSTOP.EXE` вызывает `SYS_QUEUE_STOP`, поэтому очередь прекращается сразу и shell получает управление обратно. Команда `execq-stop` полезна, когда shell уже имеет управление; во время синхронного `execq` shell не выполняет новый ввод до завершения очереди.

## Ожидаемая последовательность

Для:

```text
execq 2 QPASS.EXE QPORT.EXE
```

ожидаются две полные последовательности:

```text
QPASS: queue member executed
QPORT: port I/O test
QPORT: IN8 OK
QPORT: OUT8 OK
QPASS: queue member executed
QPORT: port I/O test
QPORT: IN8 OK
QPORT: OUT8 OK
execq: finished
```

Для `exec QFROMEXE.EXE` ожидается сообщение `QFROMEXE: starting nested queue`, затем выполнение `QPASS.EXE`, после чего сохранённый shell frame возвращает управление shell.

Для `execq forever QSTOP.EXE QPASS.EXE` ожидается `QSTOP: requesting queue stop`, после чего shell получает управление; `QPASS.EXE` не должен запускаться после остановки.


## v30: вложенная очередь и standalone EXE1

Для проверки продолжения родительской очереди используется `QNESTED.EXE`.
Запустите:

```text
execq 1 QPASS.EXE QNESTED.EXE QPASS.EXE
```

`QNESTED.EXE` из активного родителя создаёт дочернюю очередь
`QPASS.EXE -> QPORT.EXE`. Ожидаемый порядок:

```text
QPASS: queue member executed
QNESTED: starting child queue QPASS -> QPORT
QPASS: queue member executed
QPORT: port I/O test
QPORT: IN8 OK
QPORT: OUT8 OK
QPASS: queue member executed
execq: finished
```

Ключевая проверка — последний `QPASS` родительской очереди должен выполниться
после завершения дочерней очереди. Если его нет, восстановление родительского
состояния работает неправильно.

## v29: запись и дозапись файла из EXE1

`QFILE.EXE` сначала открывает `EXELOG.TXT` с `WRITE|CREATE|TRUNC`, записывает
строку и закрывает файл. Затем открывает тот же файл с `WRITE|APPEND`, дописывает
вторую строку и снова закрывает его. Запуск:

```text
exec QFILE.EXE
cat EXELOG.TXT
```

Ожидаемый файл:

```text
QFILE: first write
QFILE: append write
```

Повторный запуск `QFILE.EXE` намеренно начинает с `TRUNC`, поэтому результат
остаётся детерминированным и каждый запуск заново проверяет как обычную запись,
так и дозапись.


### Исправление v30

Обязательно проверить оба режима запуска `QNESTED.EXE`:

```text
execq 1 QPASS.EXE QNESTED.EXE QPASS.EXE
exec QNESTED.EXE
```

В первом случае проверяется возврат в родительскую очередь. Во втором случае
проверяется создание верхнеуровневой очереди непосредственно из EXE1 и
последующий возврат в shell. Ошибка `return error` для второго запуска в v29
была вызвана неверным определением `from_exe` и запретом `SYS_EXEC_QUEUE` при
`exe_active && !queue_active`.


## SYS_FILE_SIZE

`QSIZE.EXE` проверяет системный вызов `SYS_FILE_SIZE=17` непосредственно из Ring 3.

Тесты:

```text
filesize README.TXT
syscall 17 README.TXT
exec QSIZE.EXE
```

Для существующего файла EAX содержит точный размер в байтах. Для отсутствующего файла возвращается `0xffffffff`. Вызов не требует открытия fd и не оставляет дескриптор открытым.
