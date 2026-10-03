# ToyOS v67.11-B FIX60ZHD

Основа: подтверждённый на реальной системе FIX60ZG (стабильная совместная работа RT8 + MT2).

Исправление:
- удалена зависимость решения SONARVWR от экспериментального foreground→MT scheduling из FIX60ZH/ZHB/ZHC: kernel взят из стабильного FIX60ZG;
- SONARVWR выводит bounded snapshot: не более 8 последних валидных записей `/SONAR.LOG`;
- viewer сохраняет позиционное PREAD-чтение и не влияет на состояние SONARLOG;
- добавлен TESTVWR.TST с многократными запусками viewer на фоне MT+RT;
- TESTVWR включён в ACCEPT.TXT и фактический mkfat16 payload;
- acceptance содержит 45 тестов.
