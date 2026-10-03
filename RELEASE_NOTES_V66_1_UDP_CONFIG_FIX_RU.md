# Toy OS v66.1 — исправление конфигурации UDP-телеметрии

## Причина исправления

В v66 `NETDRV.EXE` продолжал загружать старый `NET.CFG` v65.15 с адресами `10.0.2.15/10.0.2.2`. Поэтому команда
`exec NETDRV.EXE TELEMETRY SLAVE1 192.168.100.10` вычисляла маршрут через старый шлюз `10.0.2.2` и показывала `ARP failed`.

## Исправление

`NETDRV.EXE` теперь автоматически выбирает конфигурацию по роли:

- `MASTER` → `NET_MAST.CFG` → `192.168.100.10`
- `SLAVE1` → `NET_SLV1.CFG` → `192.168.100.11`
- `SLAVE2` → `NET_SLV2.CFG` → `192.168.100.12`

Старый `NET.CFG` не изменён, поэтому прежние TCP/HTTP тесты остаются совместимыми.

## QEMU/TAP

На каждой машине можно использовать одно и то же имя TAP:

```bat
qemu-system-i386.exe ^
  -drive format=raw,file=build/disk.img ^
  -netdev tap,id=net0,ifname=tap0,script=no,downscript=no ^
  -device ne2k_isa,netdev=net0
```

Важно: все три QEMU должны находиться в общей Ethernet-сегментированной сети (через Windows Bridge/TAP), а IP Toy OS должны быть различными.
