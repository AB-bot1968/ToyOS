# Проверка Windows-native publication helper FIX60ZEI

- Причина предыдущего отказа: `git_preflight.bat` вызывал POSIX preflight, а найденный на Windows `sh.exe` не имел `sha256sum`.
- Исправление: SHA-256 manifest проверяется нативно через Windows PowerShell/.NET.
- Кириллица удалена из BAT-команд, влияющих на Git identity; identity задаётся PowerShell UTF-8 BOM.
- Frozen manifest: 1109 файлов.
- `src/kernel.c`: `3af4e5b2a9c8610eaee3efb1b533836dadf843e1b212922c9fee354c860dd001`.
- `TST/TESTMRT.TST`: `8bff965aa737de277a2bc901456508ff0199ae1ec350c85196af7f4f78302985`.
- Runtime/TST baseline не изменялся.

## Финальный интеграционный тест публикации

Проверен сценарий с уже существующим `main`, содержащим bootstrap-коммит. `publish_to_existing_github.sh` создал один fast-forward release commit, tag `v67.11-B-FIX60ZEI` и push без force. После нового clone `sha256sum -c FIX60ZEI_FROZEN_CONTENT_SHA256.txt` и `git_preflight.sh` прошли. Повторный запуск не создал новый commit и завершился как idempotent (`Everything up-to-date`).

Для обеспечения побайтовой воспроизводимости `.gitattributes` переведён в publication-layer и содержит `* -text`; сам файл исключён из frozen manifest. Manifest защищает 1109 файлов стабильного дерева.
