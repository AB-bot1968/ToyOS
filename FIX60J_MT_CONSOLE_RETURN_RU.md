# FIX60J — возврат foreground-консоли при SONARDRV/EXECMT

Исправления:
- background EXECMT не пишет scheduler-сообщения в foreground console;
- добавлен cooperative MT yield (syscall 74), сохраняющий текущий MT context как READY и немедленно восстанавливающий foreground shell context/CR3;
- SONARDRV выполняет MT yield после успешно опубликованного SONAR sample и heartbeat, поэтому бесконечный цикл опроса не удерживает foreground до следующего PIT quantum;
- сохранён полностью polled UART transport без IRQ4;
- RX flush очищает software ring и bounded hardware FIFO;
- устаревший FIX60E static test обновлён: он проверяет семантику усиленного flush, а не старую однострочную реализацию;
- каталог build не входит в исходный архив.

Проверка на стенде:
1. `execmt SONARDRV.EXE` должен вернуть `toy0>` и не печатать фоновых сообщений.
2. Отправить одну строку клиенту/SONARSIM: sample должен обработаться, после чего `toy0>` остаётся интерактивным.
3. Ввести команду `mtstat` или `ps` сразу после первой строки — команда должна выполниться без дополнительного Enter/ожидания.
4. Повторить не менее 10 SONAR запросов; foreground shell должен оставаться интерактивным между запросами.
