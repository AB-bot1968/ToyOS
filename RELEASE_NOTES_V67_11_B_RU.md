# ToyOS v67.11-B — RT BACKGROUND TASK CONTROL

База: v67.11-A FILESYSTEM OFFSET.

## Изменения

- F10 запускает подготовленные RT-задачи в detached/background режиме и сразу возвращает shell.
- `rtstat stop` завершает все активные RT-задачи SENSOR1..SENSOR4.
- `rtstat stop sensor1` завершает только SENSOR1; SENSOR2..SENSOR4 продолжают работу.
- Имена SENSOR1..SENSOR4 в команде `rtstat stop` регистронезависимы.
- `rtstat watch` теперь получает ESC как событие foreground shell и завершает только watch; фоновые RT-задачи не останавливаются.
- Для управления остановкой расширена семантика существующего `SYS_RT_STATUS=43`; номер syscall не изменён.
- Нового syscall номера на этапе B нет.
- Остановка не очищает RTSTAT и не стирает последнюю RT-сессию; это позволяет анализировать состояние после остановки.

## Сохранено

- i386 protected mode, freestanding C, без libc.
- Syscall ABI 1..48 и существующие номера не перенумеровываются.
- Case-insensitive command input.
- FAT16 начинаетcя с LBA 512 из v67.11-A.
