# ToyOS v67.11-B FIX60T — read-only SONAR.LOG viewer

FIX60T основан строго на принятом FIX60S.

- Добавлен `SONARVWR.EXE` — отдельный Ring-3 просмотрщик `/SONAR.LOG`.
- Просмотрщик открывает журнал только с `MODE_READ`; write/create/truncate syscalls и режимы отсутствуют.
- Проверяются точный размер 4160 bytes, CRC/формат обоих заголовков и CRC каждой выводимой записи.
- Из двух валидных заголовков выбирается более новое поколение тем же `tlog_newer()`, что и в SONARLOG.
- Кольцо выводится в хронологическом порядке: от самой старой валидной записи к самой новой.
- Формат строки: `sequence time_us x_mm y_mm z_mm status generation`.
- Новых syscall и изменений формата SONAR.LOG нет; SONARDRV/UARTRX/SONARSIM не изменены.

Запуск: `exec SONARVWR.EXE`.

## FIX60T correction: FAT 8.3
- Имя исполняемого файла просмотрщика исправлено на `SONARVWR.EXE` (8.3).
- Общий `check_fix60c_fat83_names.sh` включён как обязательная зависимость FIX60T gate.
- Единственное разрешённое исключение общего правила 8.3 остаётся `AUTOSTART.SH`.
