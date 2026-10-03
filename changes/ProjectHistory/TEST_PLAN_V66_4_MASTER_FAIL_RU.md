# План тестирования v66.4

## 1. Сборка

В W64DevKit x86:

```bash
./check109.sh
./check110.sh
./check111.sh
./check112.sh
./build.sh
```

Ожидается `PASS` на проверках и успешное создание `build/disk.img`.

## 2. Master без Slave

Запустить QEMU с TAP и NE2000.

В Toy OS:

```text
exec NETDRV.EXE TELEMETRY MASTER 5000
```

Ожидается:

```text
NETDRV: NE2000 init
NETDRV: UDP MASTER listening
```

Команда не должна завершаться строкой `FAIL` самопроизвольно.

## 3. Проверка остановки

Нажать `ESC`.

Ожидается возврат в shell без `FAIL`:

```text
NETDRV: UDP MASTER stopped
toy0>
```

## 4. Master + Slave1

На ПК Slave:

```text
exec NETDRV.EXE TELEMETRY SLAVE1 10.66.1.2
```

После установления маршрута Master должен принять пакеты от `10.66.2.2` и увеличить `RX`.

## 5. Master + Slave2 на той же второй машине

Третья машина не нужна.

После остановки Slave1 можно проверить:

```text
exec NETDRV.EXE TELEMETRY SLAVE2 10.66.1.2
```

Master должен распознавать источник `10.66.2.3` как SLAVE2.

## 6. Проверка устойчивости Master

Оставить Master работать 1–2 минуты без Slave.

Любые посторонние Ethernet-пакеты из Windows/TAP не должны приводить к `FAIL` и завершению процесса.
