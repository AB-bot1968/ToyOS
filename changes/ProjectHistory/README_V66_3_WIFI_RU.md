# Toy OS v66.3

## UDP telemetry через Wi-Fi без L2 Bridge

v66.3 продолжает v66.2. Основная цель — передать UDP-телеметрию между двумя Toy OS на разных Windows 7 ПК через обычную Wi-Fi сеть без создания Windows Network Bridge.

### Архитектура

```text
Toy OS Master 10.66.1.2
        |
       NE2000
        |
      QEMU
        |
      tap0
  10.66.1.1/24
        |
  Windows 7 routing
        |
       Wi-Fi
        |
  Windows 7 routing
        |
      tap0
  10.66.2.1/24
        |
      QEMU
        |
       NE2000
        |
Toy OS Slave1 10.66.2.2
```

### Главное исправление

В v66.2 Slave пытался ARP-ить адрес удалённого Master. В v66.3 Slave определяет, что Master находится за пределами локальной подсети, и делает ARP локального Windows gateway.

Например:

```text
Master: 10.66.1.2
Slave:  10.66.2.2
Slave gateway: 10.66.2.1
```

Поэтому Slave выполняет:

```text
ARP 10.66.2.1
```

а затем отправляет IPv4/UDP с назначением:

```text
10.66.1.2:5000
```

Windows маршрутизирует этот пакет через Wi-Fi.

### Необходимые программы

- Windows 7 x86/x64.
- TAP-Windows 9.21.2.
- QEMU 5.1.92 / 5.2 RC с `tap` и `ne2k_isa`.
- W64DevKit x86 для сборки Toy OS.

### Команды QEMU

Master и Slave используют:

```bat
qemu-system-i386.exe ^
  -drive format=raw,file=build\disk.img ^
  -netdev tap,id=net0,ifname=tap0,script=no,downscript=no ^
  -device ne2k_isa,netdev=net0
```

### Команды Toy OS

Master:

```text
exec NETDRV.EXE TELEMETRY MASTER 5000
```

Slave1:

```text
exec NETDRV.EXE TELEMETRY SLAVE1 10.66.1.2
```

Slave2:

```text
exec NETDRV.EXE TELEMETRY SLAVE2 10.66.1.2
```

### Проверка

Полный порядок находится в `TEST_PLAN_V66_3_WIFI_RU.md`.
