# Проверка GitHub shell-auth FIX для FIX60ZEI

Проверено 2026-10-03.

- `sh -n publish_to_existing_github.sh`: PASS.
- `git_preflight.sh`: PASS.
- Frozen manifest: PASS, 1109 entries.
- `src/kernel.c` SHA-256 соответствует стабильному FIX60ZEI.
- 45 acceptance-тестов присутствуют.
- Локальный сценарий на существующем bare remote: PASS.
- Первый запуск: clone -> write-auth dry-run -> copy -> preflight -> commit -> tag -> push: PASS.
- Второй запуск: no new content commit, tag unchanged, push up-to-date: PASS.
- Force push не используется.

Реальная GitHub-аутентификация зависит от credential helper/браузера пользователя и не может быть воспроизведена в локальном bare-remote тесте; именно поэтому она вынесена в ранний отдельный этап с dry-run и диагностическим логом.

## Дополнение: переход на SSH

После успешной ручной проверки `ssh -T git@github.com` shell-публикация переведена на SSH remote. Новый `publish_to_existing_github.sh` не использует HTTPS/GCM. До создания release commit выполняются SSH `git ls-remote` и write `git push --dry-run`.
