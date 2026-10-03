rem d:\qemu\qemu-system-i386.exe  -drive file=build\toy_os.img,format=raw,if=ide -serial COM11
rem -machine pc -cpu pentium3 -m 64M
rem d:\qemu\qemu-system-i386.exe -m 64M -drive file=build\toy_os.img,format=raw -chardev serial,id=sonar,path=COM11 -serial chardev:sonar
rem d:\qemu\qemu-system-i386.exe -m 64M -drive file=build\toy_os.img,format=raw -chardev serial,id=sonar,path=\\.\COM11 -serial chardev:sonar
rem d:\qemu\qemu-system-i386.exe -drive file=build\toy_os.img,format=raw -serial COM11
rem     qemu-system-i386.exe -drive file=build\toy_os.img,format=raw -serial COM11
rem d:\qemu\qemu-system-i386.exe -drive file=build\toy_os.img,format=raw -serial \\.\COM11
rem d:\qemu\qemu-system-i386.exe --version > VVV.txt
rem d:\qemu\qemu-system-i386.exe -drive file=build\toy_os.img,format=raw -chardev serial,id=toyserial,path=COM11 -serial chardev:toyserial
rem d:\qemu\qemu-system-i386.exe -drive format=raw,file=build\toy_os.img,format=raw -serial COM1

rem d:\qemu\qemu-system-i386.exe -hda build\toy_os.img -m 16M -serial COM1
d:\qemu\qemu-system-i386.exe -drive format=raw,file=build\toy_os.img -m 1024M -chardev socket,id=sonar,host=127.0.0.1,port=4555,server,nowait,nodelay -device isa-serial,chardev=sonar,iobase=0x3f8,irq=4
rem     qemu-system-i386.exe -drive format=raw,file=build\toy_os.img -m 16M -chardev socket,id=sonar,host=127.0.0.1,port=4555,server,nowait,nodelay -device isa-serial,chardev=sonar,iobase=0x3f8,irq=4