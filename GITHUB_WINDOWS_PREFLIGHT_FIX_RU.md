# Исправление Windows-preflight для GitHub-публикации FIX60ZEI

Во время первого запуска `publish_to_existing_github.bat` на Windows было выявлено, что прежний BAT-wrapper вызывал POSIX `git_preflight.sh`, который требовал внешнюю утилиту `sha256sum`. Обычный `cmd.exe` Windows не обязан иметь эту утилиту, поэтому публикация корректно останавливалась до любых Git-изменений.

Текущий вариант использует отдельный `git_preflight_windows.ps1`. SHA-256 вычисляется через .NET `SHA256Managed`, доступный Windows PowerShell, поэтому `sha256sum` больше не требуется. Manifest из 1109 frozen-файлов проверяется теми же ожидаемыми SHA-256.

BAT-файлы содержат только ASCII, а строки с именем автора хранятся в PowerShell-файлах UTF-8 с BOM. Это предотвращает искажение `Ботнев Александр Валерьевич` при разборе BAT в старом `cmd.exe`. Git identity задаётся отдельным `set_git_identity_windows.ps1` и действует только внутри временного clone.

Runtime-код ToyOS, `src/kernel.c`, TST и frozen manifest не изменены.
