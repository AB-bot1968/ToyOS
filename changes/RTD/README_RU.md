# RTD — документация изменений

Все изменения RTD хранятся отдельно от исходников в `changes/RTD/`.

Структура включает:

- `Stage1/` — запуск обычного EXE1 как detached RT-задачи;
- `Stage2/` — применение `period/deadline/priority` и `SYS_RT_WAIT`;
- `Stage3/` — несколько RT-задач;
- `Stage3_F10/` — подготовка RT-команд и запуск по F10;
- `Stage3_F10_FIX1/` — исправление Ring-3 page fault из-за mutable shell state.
- `Stage4/` — Stage 4.1: чистый компаратор priority с unit-test; фактическое priority arbitration ещё не подключено.

Исторические Markdown-файлы проекта зеркалируются в `changes/ProjectHistory/`.
Корневые копии сохранены для совместимости с существующими regression-checks.

- Stage 5.2 FIX1 — уточнение поведения deadline miss, wrap-safe deadline ordering и диагностический exact-scenario test.
