# Верификация Stage 4.2

Проверено статически:

- `rt_priority_compare()` подключён к `rt_pick_ready()`;
- priority сравнивается первым;
- deadline используется только при равном priority;
- RT slot id используется как последний deterministic tie-break;
- Stage 4.1 comparator test продолжает проходить;
- добавлен отдельный Stage 4.2 selection model test.

Сборка исходников и runtime должны быть проверены отдельно в штатном W64DevKit/target environment.
