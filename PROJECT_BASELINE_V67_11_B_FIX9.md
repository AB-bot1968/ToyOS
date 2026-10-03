# ToyOS v67.11-B-FIX9 baseline

Основа: v67.11-B-FIX8.

Исправления RTSTAT deadline/DUMP:
- deadline считается пропущенным только если завершение произошло СТРОГО ПОЗЖЕ absolute deadline; завершение точно на deadline является своевременным;
- та же строгая проверка используется при timer-side фиксации miss и в SYS_RT_WAIT;
- это устраняет ложные MISS у period=deadline=10 ms на границе одного 100-Hz RT tick;
- no-backlog правило сохранено: следующий release, совпадающий с текущим tick, не считается SKIP;
- RTSTAT DUMP теперь сохраняет в каждой строке cumulative MISS/SKIP snapshot, то есть семантика колонок совпадает с RTSTAT/RTSTAT WATCH;
- редкие события больше не выглядят как постоянные нули только потому, что последние 8 jobs сами не породили новый miss/skip.

Не изменены: FAT16 start LBA 512, layout диска, syscall ABI, регистронезависимость команд/параметров, detached RT model, 100-Hz RT clock.
