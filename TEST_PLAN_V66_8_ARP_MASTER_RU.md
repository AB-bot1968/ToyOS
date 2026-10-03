# Тест-план v66.8

1. Собрать проект в Windows 7 + W64DevKit x86:

```bat
build.bat
```

2. На ПК1 запустить QEMU с `tap0` и `ne2k_isa`, затем:

```text
exec NETDRV.EXE TELEMETRY MASTER 5000
```

3. На ПК2 запустить QEMU с `tap0` и `ne2k_isa`, затем:

```text
exec NETDRV.EXE TELEMETRY SLAVE1 10.66.1.2
```

4. На Slave ожидается:

```text
ARP WAIT 10.66.2.1
ARP OK
UDP TX 1
UDP TX 2
...
```

5. На Master при первом ARP-запросе Windows ожидается:

```text
NETDRV: ARP reply 10.66.1.2
```

После этого Master должен начать получать UDP и увеличивать RX-счётчик.

6. Нажать ESC на Master: он должен завершить режим и вернуть `toy0>`.

7. Выключить Master: Slave не должен завершаться и должен продолжать повторные ARP/TX попытки.
