# FIX60Q: SONAR стенд без Windows COM/com0com

Windows native COM backend QEMU исключается из основной приемки. QEMU предоставляет
гостю тот же ISA 16550 COM1, но host side подключен к localhost TCP stream.

1. Запустить QEMU:

    qemu-system-i386.exe -drive format=raw,file=build\toy_os.img -m 16M ^
      -chardev socket,id=sonar,host=127.0.0.1,port=4555,server,nowait,nodelay ^
      -device isa-serial,chardev=sonar,iobase=0x3f8,irq=4

2. После старта QEMU запустить новый SONARSIM:

    SONARSIM.EXE --tcp 127.0.0.1 4555 --slave 1 --mode normal ^
      --xyz 123456 -654321 2000000000

3. Убедиться в `READY bounded-socket-io`.
4. В ToyOS сначала `execmt UARTRX.EXE`, затем после длительной проверки
   `mtstop ALL`, `uartreset`, `execmt SONARDRV.EXE`.

TCP является только host transport QEMU<->SONARSIM. Внутри гостя остается COM1
0x3F8 и тот же Modbus RTU byte stream; исходный код SONARDRV не зависит от TCP.
