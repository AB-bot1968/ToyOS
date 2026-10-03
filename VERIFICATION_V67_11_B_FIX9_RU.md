# Проверка ToyOS v67.11-B-FIX9

1. Статическая/host-проверка `check_rtstat_deadline_dump_fix9.sh`: PASS.
2. `kernel.c` компилируется `gcc -m32 -ffreestanding ...`: PASS (только существующее предупреждение unused fat_find_free_dir).
3. Логика deadline: now==deadline -> on-time; now>deadline -> MISS.
4. Логика release: candidate==now -> 0 SKIP; candidate<now -> stale releases пропускаются.
5. DUMP: MISS/SKIP — накопительные snapshots на момент каждого завершённого job, как в RTSTAT.

Runtime-тест в штатной i686/W64DevKit+QEMU среде:
- запустить только `SENSOR1.EXE 10 10 1`;
- наблюдать RTSTAT 20-30 секунд: для короткой SENSOR1 без конкурентов MISS/SKIP должны оставаться 0;
- RTSTAT DUMP должен показывать те же накопительные MISS/SKIP snapshots (обычно 0/0 в этом тесте);
- затем запустить несколько RT-задач/нагрузку и убедиться, что при реальном опоздании MISS/SKIP появляются согласованно в RTSTAT и DUMP.
