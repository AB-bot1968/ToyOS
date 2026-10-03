# Toy OS v66.8 — исправление ARP Master

## Причина

Slave1 уже отправлял UDP через маршрутизируемый Wi-Fi путь, но Windows на ПК1 перед передачей пакета в TAP должна была получить MAC-адрес Toy OS Master по ARP. Master не отвечал на ARP request для собственного IP 10.66.1.2, поэтому IP/UDP кадр не попадал в NE2000 Master.

## Исправление

В NETDRV добавлен минимальный ARP responder для режима TELEMETRY MASTER. Ответ формируется только на ARP request, где target IPv4 совпадает с локальным IP Master. Остальные ARP/TCP/HTTP механизмы v66.7 не изменены.

После ответа Master пишет диагностическую строку `NETDRV: ARP reply 10.66.1.2`; это только событие, история телеметрии не накапливается.

## Тест

На ПК1 запустить MASTER, на ПК2 SLAVE1. При первом прохождении Windows ARP должен попасть в Master, после чего Slave должен продолжить `UDP TX`, а Master — увеличить `RX` для SLAVE1.
