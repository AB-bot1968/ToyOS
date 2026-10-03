# Проверка v67.11-B-FIX11

Исправлены две причины ложного накопления MISS/SKIP для SENSOR1 10 10 1.

1. `rt_next_release_after()` ошибочно пропускала release, совпадающий с `now`, если отставание было кратно периоду. Для периода 1 tick пример `candidate=101, now=105` раньше давал next=106, skipped=5; правильно next=105, skipped=4.
2. При исчерпании runtime budget текущая единственная RT-задача передавала CPU shell до выполнения `SYS_RT_WAIT`. Для period=deadline=1 tick это могло искусственно растянуть короткий job через следующий PIT tick. Теперь единственная READY RT-задача продолжает выполняться до `SYS_RT_WAIT`.

Host checks:
- test_rt_period_release: OK, включая exact-grid и wrap-around.
- kernel.c: gcc -m32 -ffreestanding -fsyntax-only: OK.
- build/ в релизном дереве отсутствует.

Runtime regression:
Запустить только SENSOR1.EXE 10 10 1, затем RTSTAT WATCH 30-60 секунд и RTSTAT DUMP. Для короткой SENSOR1 ожидается отсутствие систематического роста MISS/SKIP.
