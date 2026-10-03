#!/bin/sh
# Строгий shell mode: прекращаем сборку при любой ошибке и при unset variable.
set -eu
rm -rf build
mkdir -p build

# NET.CFG является частью исходников. Всегда копируем свежую конфигурацию
# из src в build перед упаковкой FAT16, чтобы build не зависел от старого файла.
cp src/NET.CFG build/NET.CFG
cp NET_MAST.CFG build/NET_MAST.CFG
cp NET_SLV1.CFG build/NET_SLV1.CFG
cp NET_SLV2.CFG build/NET_SLV2.CFG
# FIX60U: все runtime-тесты и manifest хранятся в плоском каталоге TST.
mkdir -p build/TST
for t in TST/*.TST TST/ACCEPT.TXT; do
    cp "$t" "build/TST/`basename "$t"`"
done
# FIX60ZEH: test-source guard. Never continue a build if the staged TST copy
# differs from the source tree; runtime FAIL output also prints the bytes/FNV
# fingerprint of the file that was actually interpreted from FAT.
for t in TST/*.TST TST/ACCEPT.TXT; do
    cmp -s "$t" "build/TST/`basename "$t"`" || { echo "ERROR: staged test differs from source: $t" >&2; exit 1; }
done

# Требуем именно 32-битный target W64DevKit: kernel ABI = i386.
case "`gcc -dumpmachine`" in
    i[3-6]86-*|i686-*) ;;
    *) echo "ERROR: use the x86 W64DevKit (gcc target must be i686)."; exit 1;;
esac

# --32 = i386 assembler.
ASFLAGS="--32"

# -Os уменьшает kernel image.
# -ffreestanding запрещает предполагать hosted OS.
# -fno-pie нужен для абсолютных адресов raw kernel.
# -fno-stack-protector убирает зависимость от stack-protector runtime.
# -fno-asynchronous-unwind-tables и -fno-unwind-tables убирают ненужные metadata.
# -fno-builtin предотвращает скрытые вызовы libc.
# -fno-tree-vectorize/-fno-tree-slp-vectorize запрещают автоматическую SIMD-векторизацию.
# -mno-sse/-mno-sse2/-mno-mmx/-mno-80387 запрещают инструкции FPU/SSE/MMX:
# в ядре ещё не настроен контекст FPU, поэтому Ring 3 не должен получить
# скрытый MOVAPS/MOVDQA и не должен зависеть от состояния CR0/CR4.
# -nostdinc и -nostdlib полностью исключают стандартные headers/CRT/libc.
CFLAGS="-Os -ffreestanding -fno-pie -fno-stack-protector -fno-asynchronous-unwind-tables -fno-unwind-tables -fno-builtin -fno-tree-vectorize -fno-tree-slp-vectorize -mno-sse -mno-sse2 -mno-mmx -mno-80387 -nostdinc -nostdlib"

# 1. BIOS boot sector.
as $ASFLAGS src/boot.S -o build/boot.o
# PE/COFF 32-bit mode; image base 0; _start — единственный entry point.
ld -m i386pe --image-base 0 -e _start -T boot.ld -o build/boot.pe build/boot.o
# Извлекаем raw 512-byte boot sector.
objcopy --only-section=.text -O binary build/boot.pe build/boot.bin
test `wc -c < build/boot.bin` -eq 512
# FIX53: LBA1 layout sector carries a 64-byte descriptor followed by a
# compact loader. The descriptor is written after FAT geometry is known.
as $ASFLAGS src/layout_loader.S -o build/layout_loader.o
ld -m i386pe --image-base 0 -e layout_loader_entry -T layout_loader.ld -o build/layout_loader.pe build/layout_loader.o
objcopy --only-section=.text -O binary build/layout_loader.pe build/layout_loader.bin
test `wc -c < build/layout_loader.bin` -eq 512

# 2. Kernel + отдельный Ring-3 shell object.
gcc $CFLAGS -c src/kernel.c -o build/kernel.o
gcc $CFLAGS -c src/user_shell.c -o build/user_shell.o
gcc $CFLAGS -c src/modbus_rtu.c -o build/modbus_rtu.o
gcc $CFLAGS -c src/sonar_core.c -o build/sonar_core.o
gcc $CFLAGS -c src/telemetry_log.c -o build/telemetry_log.o
# v62: общий модуль синтаксиса путей. Он не зависит от FAT16 и не дублирует
# логику в kernel.c, поэтому дальнейшие команды работы с путями используют
# один проверенный алгоритм.
gcc $CFLAGS -c src/path.c -o build/path.o
gcc $CFLAGS -c src/hello.c -o build/hello.o
gcc $CFLAGS -c src/comdrv.c -o build/comdrv.o
# v57: LOADER.EXE — пользовательский оркестратор COMDRV -> полученный EXE1.
gcc $CFLAGS -c src/loader.c -o build/loader.o
# v58 NETDRV: минимальный Ring-3 NE2000/ARP/IPv4/TCP/HTTP клиент.
gcc $CFLAGS -c src/netdrv.c -o build/netdrv.o
# v65: VGADRV.EXE — Ring-3 VGA framebuffer driver. Картинка не входит
# в kernel: драйвер читает обычный FAT16-файл по аргументу EXEC_ARGS.
gcc $CFLAGS -c src/vgadrv.c -o build/vgadrv.o
# FIX34: diagnostic current-process identity probe.
gcc $CFLAGS -c src/procid.c -o build/procid.o
gcc $CFLAGS -c src/argvdiag.c -o build/argvdiag.o
gcc $CFLAGS -c src/leakfd.c -o build/leakfd.o
gcc $CFLAGS -c src/supervis.c -o build/supervis.o
gcc $CFLAGS -c src/hbeatok.c -o build/hbeatok.o
gcc $CFLAGS -c src/sonardrv.c -o build/sonardrv.o
gcc $CFLAGS -DSONAR_TEST_INJECT=1 -c src/sonardrv.c -o build/sonardrv_test.o
gcc $CFLAGS -c src/uartrx.c -o build/uartrx.o
gcc $CFLAGS -DUART_RX_TEST_INJECT=1 -c src/uartrx.c -o build/uartrx_test.o
gcc $CFLAGS -c src/sonarlog.c -o build/sonarlog.o
gcc $CFLAGS -c src/sonarpub.c -o build/sonarpub.o
# FIX60T: read-only SONAR.LOG viewer.
gcc $CFLAGS -c src/sonarview.c -o build/sonarview.o
gcc $CFLAGS -c src/hangwd.c -o build/hangwd.o
gcc $CFLAGS -c src/rconce.c -o build/rconce.o
gcc $CFLAGS -DEXIT_CODE=0 -c src/exit_test.c -o build/exit0.o
gcc $CFLAGS -DEXIT_CODE=7 -c src/exit_test.c -o build/exit7.o
# FIX36: deterministic normal exit-status probes.
# FIX35: deterministic Ring-3 exception containment probes.
gcc $CFLAGS -DFAULT_KIND=6 -c src/fault_test.c -o build/faultud.o
gcc $CFLAGS -DFAULT_KIND=13 -c src/fault_test.c -o build/faultgp.o
gcc $CFLAGS -DFAULT_KIND=14 -c src/fault_test.c -o build/faultpf.o
# v67 RT foundation: RTD is a normal Ring-3 EXE1 launcher; SENSOR remains a
# completely ordinary EXE1 program so the RT layer does not require special
# application binaries.
gcc $CFLAGS -c src/rtd.c -o build/rtd.o
gcc $CFLAGS -c src/rt_sensor.c -o build/rt_sensor.o
gcc $CFLAGS -DRT_SENSOR_DIAG_ID=1 -c src/rt_sensor_diag.c -o build/rt_sensor1.o
gcc $CFLAGS -DRT_SENSOR_DIAG_ID=2 -c src/rt_sensor_diag.c -o build/rt_sensor2.o
gcc $CFLAGS -DRT_SENSOR_DIAG_ID=3 -c src/rt_sensor_diag.c -o build/rt_sensor3.o
gcc $CFLAGS -DRT_SENSOR_DIAG_ID=4 -c src/rt_sensor_diag.c -o build/rt_sensor4.o
gcc $CFLAGS -DRT_SENSOR_DIAG_ID=5 -c src/rt_sensor_diag.c -o build/rt_sensor5.o
gcc $CFLAGS -DRT_SENSOR_DIAG_ID=6 -c src/rt_sensor_diag.c -o build/rt_sensor6.o
gcc $CFLAGS -DRT_SENSOR_DIAG_ID=7 -c src/rt_sensor_diag.c -o build/rt_sensor7.o
gcc $CFLAGS -DRT_SENSOR_DIAG_ID=8 -c src/rt_sensor_diag.c -o build/rt_sensor8.o
gcc $CFLAGS -c src/rt_deadline_diag.c -o build/rt_deadline_diag.o
gcc $CFLAGS -c src/rt_deadline_miss_diag.c -o build/rt_deadline_miss_diag.o
gcc $CFLAGS -c src/rt_period_skip_diag.c -o build/rt_period_skip_diag.o
gcc $CFLAGS -c src/rt_timebase_diag.c -o build/rt_timebase_diag.o
gcc $CFLAGS -c src/rt_jitter_diag.c -o build/rt_jitter_diag.o

# Additional EXE1 queue-validation programs. Each is linked independently at
# the same user image address because the kernel executes queue members
# sequentially, never concurrently.
for qsrc in src/queue_tests/qpass.c src/queue_tests/qport.c src/queue_tests/qfromexe.c src/queue_tests/qstop.c src/queue_tests/qnested.c src/queue_tests/qfile.c src/queue_tests/qsize.c; do
    qbase=`basename "$qsrc" .c`
    echo "[EXE-TEST] compiling $qsrc"
    gcc $CFLAGS -c "$qsrc" -o "build/${qbase}.o"
done

# v39: one compact EXE1 source is compiled 25 times.  Every instance is
# loaded into a distinct physical 1-MiB image slot and receives its own CR3.
for i in `seq 1 25`; do
    mtid=`printf '%02d' "$i"`
    echo "[EXECMT-TEST] compiling task ${mtid}"
    gcc $CFLAGS -DEXECMT_ID=$i -c src/execmt_task.c -o "build/mt${mtid}.o"
done
as $ASFLAGS src/isr.S -o build/isr.o

# Kernel .rdata переименовывается в .krodata: это supervisor-only read-only data.
# User .rdata оставляем отдельным: linker помещает его в page-aligned .urodata.
objcopy --rename-section .rdata=.krodata,alloc,load,readonly,data,contents,code build/kernel.o build/kernel.coff.o

# Линкуем kernel, user text, user rodata и ISR. liker.ld page-aligns user sections.
ld -m i386pe --image-base 0 -e _start -T liker.ld -o build/kernel.pe build/kernel.coff.o build/user_shell.o build/modbus_rtu.o build/sonar_core.o build/telemetry_log.o build/path.o build/isr.o

# В raw image нужны .text + .utext + .urodata вместе с промежуточными gap,
# потому что виртуальные адреса user sections должны остаться правильными.
objcopy --only-section=.text --only-section=.utext --only-section=.urodata --only-section=.udata -O binary build/kernel.pe build/kernel.bin

# FIX53 dynamic disk-layout parameters. All are build-time configurable.
KERNEL_LBA=2
KERNEL_GROWTH_PERCENT=${KERNEL_GROWTH_PERCENT:-25}
KERNEL_RESERVE_MIN=${KERNEL_RESERVE_MIN:-64}
LAYOUT_GAP_SECTORS=${LAYOUT_GAP_SECTORS:-128}
FAT_ALIGN_SECTORS=${FAT_ALIGN_SECTORS:-256}
FAT_FREE_PERCENT=${FAT_FREE_PERCENT:-25}
KERNEL_LOAD_ADDR=32256       # 0x7e00
# Reject nonsensical build policy before any arithmetic/division is attempted.
case "$KERNEL_GROWTH_PERCENT:$KERNEL_RESERVE_MIN:$LAYOUT_GAP_SECTORS:$FAT_ALIGN_SECTORS:$FAT_FREE_PERCENT" in
    *[!0-9:]*|'') echo "ERROR: FIX53 layout policy values must be non-negative decimal integers" >&2; exit 1;;
esac
if [ "$KERNEL_RESERVE_MIN" -le 0 ] || [ "$LAYOUT_GAP_SECTORS" -le 0 ] || [ "$FAT_ALIGN_SECTORS" -le 0 ] || [ "$FAT_FREE_PERCENT" -gt 75 ]; then
    echo "ERROR: invalid FIX53 layout policy (reserve/gap/alignment >0, free-percent 0..75)" >&2
    exit 1
fi
KERNEL_LOW_MEM_CEILING=655360 # 0x000a0000: first reserved runtime region (VGA aperture)
KERNEL_LOAD_LIMIT=${KERNEL_LOAD_LIMIT:-$KERNEL_LOW_MEM_CEILING}
if [ "$KERNEL_LOAD_LIMIT" -gt "$KERNEL_LOW_MEM_CEILING" ] || [ "$KERNEL_LOAD_LIMIT" -le "$KERNEL_LOAD_ADDR" ]; then
    echo "ERROR: KERNEL_LOAD_LIMIT must be > $KERNEL_LOAD_ADDR and <= $KERNEL_LOW_MEM_CEILING" >&2
    exit 1
fi

KERNEL_BYTES=`wc -c < build/kernel.bin | tr -d ' '`
KERNEL_SECTORS=$(( (KERNEL_BYTES + 511) / 512 ))
KERNEL_RESERVE=$(( (KERNEL_SECTORS * KERNEL_GROWTH_PERCENT + 99) / 100 ))
if [ "$KERNEL_RESERVE" -lt "$KERNEL_RESERVE_MIN" ]; then KERNEL_RESERVE=$KERNEL_RESERVE_MIN; fi
KERNEL_DISK_END=$(( KERNEL_LOAD_ADDR + KERNEL_SECTORS * 512 ))
if [ "$KERNEL_DISK_END" -gt "$KERNEL_LOAD_LIMIT" ]; then
    echo "ERROR: kernel load image exceeds safe low-memory range: end=$KERNEL_DISK_END limit=$KERNEL_LOAD_LIMIT" >&2
    exit 1
fi
# Linker BSS is not present in kernel.bin, therefore check it separately. nm
# prints PE symbols with the linker-script name ___bss_end.
BSS_END_HEX=`nm build/kernel.pe | awk '/bss_end$/ {print $1; exit}'`
if [ -z "$BSS_END_HEX" ]; then echo "ERROR: cannot resolve linker bss_end symbol" >&2; exit 1; fi
BSS_END=$(( 0x$BSS_END_HEX ))
if [ "$BSS_END" -gt "$KERNEL_LOAD_LIMIT" ]; then
    echo "ERROR: kernel BSS/tables exceed safe low-memory range: bss_end=$BSS_END limit=$KERNEL_LOAD_LIMIT" >&2
    exit 1
fi
LAYOUT_MIN_FAT=$(( KERNEL_LBA + KERNEL_SECTORS + KERNEL_RESERVE + LAYOUT_GAP_SECTORS ))
FAT_LBA=$(( ((LAYOUT_MIN_FAT + FAT_ALIGN_SECTORS - 1) / FAT_ALIGN_SECTORS) * FAT_ALIGN_SECTORS ))
ACTUAL_GAP=$(( FAT_LBA - (KERNEL_LBA + KERNEL_SECTORS + KERNEL_RESERVE) ))
if [ "$ACTUAL_GAP" -lt "$LAYOUT_GAP_SECTORS" ]; then echo "ERROR: internal layout gap calculation failed" >&2; exit 1; fi
echo "[LAYOUT] kernel.bin: ${KERNEL_BYTES} bytes (${KERNEL_SECTORS} sectors)"
echo "[LAYOUT] kernel reserve=${KERNEL_RESERVE}, protected gap=${ACTUAL_GAP}, FAT16 LBA=${FAT_LBA}, alignment=${FAT_ALIGN_SECTORS}"

# 3. Host FAT16 image tools.
# These are normal hosted Win32 programs. They deliberately do not inherit the
# freestanding kernel CFLAGS above. Each invocation is checked explicitly so
# a Windows shell cannot make this stage look like a silent termination.
HOST_CFLAGS="-O2 -std=c99 -Wall -Wextra -Werror"

build_host_tool() {
    src="$1"
    out="$2"
    echo "[TOOLS] compiling $src -> $out"
    if ! gcc $HOST_CFLAGS "$src" -o "$out"; then
        echo "ERROR: host utility compilation failed: $src" >&2
        echo "ERROR: gcc target: `gcc -dumpmachine`" >&2
        exit 1
    fi
    if [ ! -f "$out" ]; then
        echo "ERROR: compiler returned success but did not create $out" >&2
        exit 1
    fi
    echo "[TOOLS] OK: $out"
}

build_host_tool tools/mkfat16.c build/mkfat16.exe
build_host_tool tools/fat16check.c build/fat16check.exe
build_host_tool tools/mkexe.c build/mkexe.exe
build_host_tool tools/mklayout.c build/mklayout.exe
build_host_tool tools/layoutcheck.c build/layoutcheck.exe
# v65.15: PNG2RAW — полностью самостоятельная C-утилита под GCC/W64DevKit.
# Внешних PNG/zlib библиотек нет: DEFLATE, Adler-32 и CRC32 встроены в исходник.
echo "[TOOLS] compiling tools/png2raw.c -> build/png2raw.exe"
if ! gcc $HOST_CFLAGS -static -static-libgcc tools/png2raw.c -o build/png2raw.exe; then
    echo "ERROR: PNG2RAW compilation failed: tools/png2raw.c" >&2
    echo "ERROR: PNG2RAW must build without external compression libraries." >&2
    exit 1
fi
test -s build/png2raw.exe || { echo "ERROR: PNG2RAW compiler returned success but did not create build/png2raw.exe" >&2; exit 1; }
cp build/png2raw.exe tools/PNG2RAW.EXE
test -s tools/PNG2RAW.EXE || { echo "ERROR: failed to install tools/PNG2RAW.EXE" >&2; exit 1; }
echo "[TOOLS] OK: tools/PNG2RAW.EXE"
# pathcheck.c — hosted тестовый драйвер. Сам модуль path.c подключаем к нему
# явно, потому что build_host_tool принимает один исходный файл. Ранее
# pathcheck.c компилировался отдельно и на этапе линковки получал
# "undefined reference" для всех функций path_*().
echo "[TOOLS] compiling tools/pathcheck.c + src/path.c -> build/pathcheck.exe"
if ! gcc $HOST_CFLAGS tools/pathcheck.c src/path.c -o build/pathcheck.exe; then
    echo "ERROR: host utility compilation failed: tools/pathcheck.c + src/path.c" >&2
    echo "ERROR: gcc target: `gcc -dumpmachine`" >&2
    exit 1
fi
test -f build/pathcheck.exe || { echo "ERROR: compiler returned success but did not create build/pathcheck.exe" >&2; exit 1; }
./build/pathcheck.exe

# Общая обёртка EXE1. После каждого linker/objcopy/mkexe шага проверяем
# существование и непустой размер промежуточного файла. Это специально
# защищает сборку W64DevKit от ситуации, когда предыдущая команда не создала
# ожидаемый RAW-файл, а следующая утилита сообщает лишь "No such file".
make_exe1() {
    raw="$1"
    out="$2"
    bss="$3"
    test -s "$raw" || { echo "ERROR: EXE1 input RAW is missing or empty: $raw" >&2; exit 1; }
    echo "[EXE1] $raw -> $out (BSS=$bss)"
    if ! ./build/mkexe.exe "$raw" "$out" "$bss"; then
        echo "ERROR: mkexe failed: input=$raw output=$out bss=$bss" >&2
        exit 1
    fi
    test -s "$out" || { echo "ERROR: mkexe reported success but did not create: $out" >&2; exit 1; }
}

# 3a. Standalone Ring-3 executable. It is linked at the exact address where
# the kernel loader will place it, so no relocation table is needed.
ld -m i386pe --image-base 0 -e _start -T program.ld -o build/hello.pe build/hello.o
objcopy --only-section=.text -O binary build/hello.pe build/hello.raw
# HELLO.EXE has no BSS; mkexe still stores the BSS field for future programs.
make_exe1 build/hello.raw build/HELLO.EXE 0
ld -m i386pe --image-base 0 -e _start -T program.ld -o build/comdrv.pe build/comdrv.o
objcopy --only-section=.text -O binary build/comdrv.pe build/comdrv.raw
make_exe1 build/comdrv.raw build/COMDRV.EXE 0
# v57: LOADER.EXE использует SYS_EXEC_QUEUE_ARGS для цепочки COMDRV -> FILE.EXE.
ld -m i386pe --image-base 0 -e _start -T program.ld -o build/loader.pe build/loader.o
objcopy --only-section=.text -O binary build/loader.pe build/loader.raw
make_exe1 build/loader.raw build/LOADER.EXE 0
# v58: NETDRV.EXE получает IP/DIRECTION/FILE через существующий SYS_EXEC_ARGS.
ld -m i386pe --image-base 0 -e _start -T program.ld -o build/netdrv.pe build/netdrv.o
objcopy --only-section=.text -O binary build/netdrv.pe build/netdrv.raw
# NETDRV использует около 5 KiB статических буферов; загрузчик очищает BSS по заголовку EXE1.
make_exe1 build/netdrv.raw build/NETDRV.EXE 8192
ld -m i386pe --image-base 0 -e _start -T program.ld -o build/vgadrv.pe build/vgadrv.o
objcopy --only-section=.text -O binary build/vgadrv.pe build/vgadrv.raw
# Буфер драйвера = 512 байт; он используется как промежуточный сектор FAT16.
make_exe1 build/vgadrv.raw build/VGADRV.EXE 512
ld -m i386pe --image-base 0 -e _start -T program.ld -o build/procid.pe build/procid.o
objcopy --only-section=.text -O binary build/procid.pe build/procid.raw
make_exe1 build/procid.raw build/PROCID.EXE 0
ld -m i386pe --image-base 0 -e _start -T program.ld -o build/argvdiag.pe build/argvdiag.o
objcopy --only-section=.text -O binary build/argvdiag.pe build/argvdiag.raw
make_exe1 build/argvdiag.raw build/ARGVDIAG.EXE 0
ld -m i386pe --image-base 0 -e _start -T program.ld -o build/supervis.pe build/supervis.o
objcopy --only-section=.text -O binary build/supervis.pe build/supervis.raw
make_exe1 build/supervis.raw build/SUPERVIS.EXE 0
ld -m i386pe --image-base 0 -e _start -T program.ld -o build/hbeatok.pe build/hbeatok.o
objcopy --only-section=.text -O binary build/hbeatok.pe build/hbeatok.raw
make_exe1 build/hbeatok.raw build/HBEATOK.EXE 0
ld -m i386pe --image-base 0 -e _start -T program.ld -o build/sonardrv.pe build/sonardrv.o build/modbus_rtu.o build/sonar_core.o
objcopy --only-section=.text -O binary build/sonardrv.pe build/sonardrv.raw
make_exe1 build/sonardrv.raw build/SONARDRV.EXE 0
ld -m i386pe --image-base 0 -e _start -T program.ld -o build/sonardrv_test.pe build/sonardrv_test.o build/modbus_rtu.o build/sonar_core.o
objcopy --only-section=.text -O binary build/sonardrv_test.pe build/sonardrv_test.raw
make_exe1 build/sonardrv_test.raw build/SONARTST.EXE 0
ld -m i386pe --image-base 0 -e _start -T program.ld -o build/uartrx.pe build/uartrx.o
objcopy --only-section=.text -O binary build/uartrx.pe build/uartrx.raw
make_exe1 build/uartrx.raw build/UARTRX.EXE 0
ld -m i386pe --image-base 0 -e _start -T program.ld -o build/uartrxt.pe build/uartrx_test.o
objcopy --only-section=.text -O binary build/uartrxt.pe build/uartrxt.raw
make_exe1 build/uartrxt.raw build/UARTRXT.EXE 0
ld -m i386pe --image-base 0 -e _start -T program.ld -o build/sonarlog.pe build/sonarlog.o build/telemetry_log.o
objcopy --only-section=.text -O binary build/sonarlog.pe build/sonarlog.raw
make_exe1 build/sonarlog.raw build/SONARLOG.EXE 0
ld -m i386pe --image-base 0 -e _start -T program.ld -o build/sonarpub.pe build/sonarpub.o
objcopy --only-section=.text -O binary build/sonarpub.pe build/sonarpub.raw
make_exe1 build/sonarpub.raw build/SONARPUB.EXE 0
ld -m i386pe --image-base 0 -e _start -T program.ld -o build/sonarview.pe build/sonarview.o build/telemetry_log.o
objcopy --only-section=.text -O binary build/sonarview.pe build/sonarview.raw
make_exe1 build/sonarview.raw build/SONARVWR.EXE 0
ld -m i386pe --image-base 0 -e _start -T program.ld -o build/hangwd.pe build/hangwd.o
objcopy --only-section=.text -O binary build/hangwd.pe build/hangwd.raw
make_exe1 build/hangwd.raw build/HANGWD.EXE 0
ld -m i386pe --image-base 0 -e _start -T program.ld -o build/rconce.pe build/rconce.o
objcopy --only-section=.text -O binary build/rconce.pe build/rconce.raw
make_exe1 build/rconce.raw build/RCONCE.EXE 0
ld -m i386pe --image-base 0 -e _start -T program.ld -o build/leakfd.pe build/leakfd.o
objcopy --only-section=.text -O binary build/leakfd.pe build/leakfd.raw
make_exe1 build/leakfd.raw build/LEAKFD.EXE 0
ld -m i386pe --image-base 0 -e _start -T program.ld -o build/exit0.pe build/exit0.o
objcopy --only-section=.text -O binary build/exit0.pe build/exit0.raw
make_exe1 build/exit0.raw build/EXIT0.EXE 0
ld -m i386pe --image-base 0 -e _start -T program.ld -o build/exit7.pe build/exit7.o
objcopy --only-section=.text -O binary build/exit7.pe build/exit7.raw
make_exe1 build/exit7.raw build/EXIT7.EXE 0
ld -m i386pe --image-base 0 -e _start -T program.ld -o build/faultud.pe build/faultud.o
objcopy --only-section=.text -O binary build/faultud.pe build/faultud.raw
make_exe1 build/faultud.raw build/FAULTUD.EXE 0
ld -m i386pe --image-base 0 -e _start -T program.ld -o build/faultgp.pe build/faultgp.o
objcopy --only-section=.text -O binary build/faultgp.pe build/faultgp.raw
make_exe1 build/faultgp.raw build/FAULTGP.EXE 0
ld -m i386pe --image-base 0 -e _start -T program.ld -o build/faultpf.pe build/faultpf.o
objcopy --only-section=.text -O binary build/faultpf.pe build/faultpf.raw
make_exe1 build/faultpf.raw build/FAULTPF.EXE 0
ld -m i386pe --image-base 0 -e _start -T program.ld -o build/rtd.pe build/rtd.o
objcopy --only-section=.text -O binary build/rtd.pe build/rtd.raw
make_exe1 build/rtd.raw build/RTD.EXE 0
ld -m i386pe --image-base 0 -e _start -T program.ld -o build/rt_sensor.pe build/rt_sensor.o
objcopy --only-section=.text -O binary build/rt_sensor.pe build/rt_sensor.raw
make_exe1 build/rt_sensor.raw build/SENSOR.EXE 0
ld -m i386pe --image-base 0 -e _start -T program.ld -o build/rt_sensor1.pe build/rt_sensor1.o
objcopy --only-section=.text -O binary build/rt_sensor1.pe build/rt_sensor1.raw
make_exe1 build/rt_sensor1.raw build/SENSOR1.EXE 0
ld -m i386pe --image-base 0 -e _start -T program.ld -o build/rt_sensor2.pe build/rt_sensor2.o
objcopy --only-section=.text -O binary build/rt_sensor2.pe build/rt_sensor2.raw
make_exe1 build/rt_sensor2.raw build/SENSOR2.EXE 0
ld -m i386pe --image-base 0 -e _start -T program.ld -o build/rt_sensor3.pe build/rt_sensor3.o
objcopy --only-section=.text -O binary build/rt_sensor3.pe build/rt_sensor3.raw
make_exe1 build/rt_sensor3.raw build/SENSOR3.EXE 0
ld -m i386pe --image-base 0 -e _start -T program.ld -o build/rt_sensor4.pe build/rt_sensor4.o
objcopy --only-section=.text -O binary build/rt_sensor4.pe build/rt_sensor4.raw
make_exe1 build/rt_sensor4.raw build/SENSOR4.EXE 0
ld -m i386pe --image-base 0 -e _start -T program.ld -o build/rt_sensor5.pe build/rt_sensor5.o
objcopy --only-section=.text -O binary build/rt_sensor5.pe build/rt_sensor5.raw
make_exe1 build/rt_sensor5.raw build/SENSOR5.EXE 0
ld -m i386pe --image-base 0 -e _start -T program.ld -o build/rt_sensor6.pe build/rt_sensor6.o
objcopy --only-section=.text -O binary build/rt_sensor6.pe build/rt_sensor6.raw
make_exe1 build/rt_sensor6.raw build/SENSOR6.EXE 0
ld -m i386pe --image-base 0 -e _start -T program.ld -o build/rt_sensor7.pe build/rt_sensor7.o
objcopy --only-section=.text -O binary build/rt_sensor7.pe build/rt_sensor7.raw
make_exe1 build/rt_sensor7.raw build/SENSOR7.EXE 0
ld -m i386pe --image-base 0 -e _start -T program.ld -o build/rt_sensor8.pe build/rt_sensor8.o
objcopy --only-section=.text -O binary build/rt_sensor8.pe build/rt_sensor8.raw
make_exe1 build/rt_sensor8.raw build/SENSOR8.EXE 0
ld -m i386pe --image-base 0 -e _start -T program.ld -o build/rt_deadline_diag.pe build/rt_deadline_diag.o
objcopy --only-section=.text -O binary build/rt_deadline_diag.pe build/rt_deadline_diag.raw
make_exe1 build/rt_deadline_diag.raw build/DEADLINE.EXE 0
ld -m i386pe --image-base 0 -e _start -T program.ld -o build/rt_deadline_miss_diag.pe build/rt_deadline_miss_diag.o
objcopy --only-section=.text -O binary build/rt_deadline_miss_diag.pe build/rt_deadline_miss_diag.raw
make_exe1 build/rt_deadline_miss_diag.raw build/RTDMISS.EXE 0
ld -m i386pe --image-base 0 -e _start -T program.ld -o build/rt_period_skip_diag.pe build/rt_period_skip_diag.o
objcopy --only-section=.text -O binary build/rt_period_skip_diag.pe build/rt_period_skip_diag.raw
make_exe1 build/rt_period_skip_diag.raw build/RTDSKIP.EXE 0
ld -m i386pe --image-base 0 -e _start -T program.ld -o build/rt_timebase_diag.pe build/rt_timebase_diag.o
objcopy --only-section=.text -O binary build/rt_timebase_diag.pe build/rt_timebase_diag.raw
make_exe1 build/rt_timebase_diag.raw build/RTTIME.EXE 0
ld -m i386pe --image-base 0 -e _start -T program.ld -o build/rt_jitter_diag.pe build/rt_jitter_diag.o
objcopy --only-section=.text -O binary build/rt_jitter_diag.pe build/rt_jitter_diag.raw
make_exe1 build/rt_jitter_diag.raw build/JITTER.EXE 0
# Тестовый графический ресурс: чистый RAW 320x200x256, ровно 64000 байт.
# Важно: mkfat16 ниже получает все входные файлы из каталога build/.
# Поэтому ресурс сначала явно копируется из исходного каталога resources/
# в build/. В предыдущей версии здесь использовался build/SPLASH.RAW,
# но сам файл в build/ не создавался, из-за чего сборка останавливалась
# именно на этапе добавления SPLASH.RAW в FAT16.
if [ ! -f resources/SPLASH.RAW ]; then
    echo "ERROR: source graphic resource is missing: resources/SPLASH.RAW" >&2
    exit 1
fi
cp resources/SPLASH.RAW build/SPLASH.RAW
if [ ! -s build/SPLASH.RAW ]; then
    echo "ERROR: failed to stage graphic resource: build/SPLASH.RAW" >&2
    exit 1
fi
SPLASH_BYTES=`wc -c < build/SPLASH.RAW | tr -d '[:space:]'`
if [ "$SPLASH_BYTES" -ne 64000 ]; then
    echo "ERROR: SPLASH.RAW must be exactly 64000 bytes; got $SPLASH_BYTES" >&2
    exit 1
fi
echo "[RESOURCE] build/SPLASH.RAW: ${SPLASH_BYTES} bytes (320x200x256 RAW, displayed 1x1 in VGA Mode 13h)"

# v65.11: AUTOSTART.SH необязателен. Если исходный файл существует, он попадёт
# в FAT16 root под фиксированным именем /AUTOSTART.SH.
AUTOSTART_FAT_ARG=""
if [ -f resources/AUTOSTART.SH ]; then
    cp resources/AUTOSTART.SH build/AUTOSTART.SH
    test -s build/AUTOSTART.SH || { echo "ERROR: failed to stage AUTOSTART.SH" >&2; exit 1; }
    AUTOSTART_FAT_ARG=" build/AUTOSTART.SH=AUTOSTART.SH"
    echo "[RESOURCE] build/AUTOSTART.SH -> /AUTOSTART.SH"
fi

# Build all queue-test EXE1 images.
for qsrc in qpass qport qfromexe qstop qnested qfile qsize; do
    ld -m i386pe --image-base 0 -e _start -T program.ld -o "build/${qsrc}.pe" "build/${qsrc}.o"
    objcopy --only-section=.text -O binary "build/${qsrc}.pe" "build/${qsrc}.raw"
    qname=`echo "$qsrc" | tr "[:lower:]" "[:upper:]"`.EXE
    make_exe1 "build/${qsrc}.raw" "build/${qname}" 0
done

# Build 25 real EXECMT EXE1 images.
for i in `seq 1 25`; do
    mtid=`printf '%02d' "$i"`
    ld -m i386pe --image-base 0 -e _start -T program.ld -o "build/mt${mtid}.pe" "build/mt${mtid}.o"
    objcopy --only-section=.text -O binary "build/mt${mtid}.pe" "build/mt${mtid}.raw"
    make_exe1 "build/mt${mtid}.raw" "build/MT${mtid}.EXE" 0
done

# Install HELLO plus every queue-validation EXE in the FAT16 root.
EXECMT_FILES=""
for i in `seq 1 25`; do
    mtid=`printf '%02d' "$i"`
    EXECMT_FILES="$EXECMT_FILES build/MT${mtid}.EXE"
done
./build/mkfat16.exe build/disk.img --part-lba "$FAT_LBA" --free-percent "$FAT_FREE_PERCENT" build/boot.bin=BOOT.BIN build/HELLO.EXE=HELLO.EXE build/COMDRV.EXE=COMDRV.EXE build/LOADER.EXE=LOADER.EXE build/NETDRV.EXE=NETDRV.EXE build/VGADRV.EXE=VGADRV.EXE build/PROCID.EXE=PROCID.EXE build/ARGVDIAG.EXE=ARGVDIAG.EXE build/SUPERVIS.EXE=SUPERVIS.EXE build/EXIT0.EXE=EXIT0.EXE build/EXIT7.EXE=EXIT7.EXE build/FAULTUD.EXE=FAULTUD.EXE build/FAULTGP.EXE=FAULTGP.EXE build/FAULTPF.EXE=FAULTPF.EXE build/RTD.EXE=RTD.EXE build/SENSOR.EXE=SENSOR.EXE build/SENSOR1.EXE=SENSOR1.EXE build/SENSOR2.EXE=SENSOR2.EXE build/SENSOR3.EXE=SENSOR3.EXE build/SENSOR4.EXE=SENSOR4.EXE build/SENSOR5.EXE=SENSOR5.EXE build/SENSOR6.EXE=SENSOR6.EXE build/SENSOR7.EXE=SENSOR7.EXE build/SENSOR8.EXE=SENSOR8.EXE build/DEADLINE.EXE=DEADLINE.EXE build/RTDMISS.EXE=RTDMISS.EXE build/RTDSKIP.EXE=RTDSKIP.EXE build/RTTIME.EXE=RTTIME.EXE build/JITTER.EXE=JITTER.EXE build/SPLASH.RAW=SPLASH.RAW$AUTOSTART_FAT_ARG build/NET.CFG=NET.CFG build/NET_MAST.CFG=NET_MAST.CFG build/NET_SLV1.CFG=NET_SLV1.CFG build/NET_SLV2.CFG=NET_SLV2.CFG build/QPASS.EXE=QPASS.EXE build/QPORT.EXE=QPORT.EXE build/QFROMEXE.EXE=QFROMEXE.EXE build/QSTOP.EXE=QSTOP.EXE build/QNESTED.EXE=QNESTED.EXE build/QFILE.EXE=QFILE.EXE build/QSIZE.EXE=QSIZE.EXE build/TST/TESTCORE.TST=TST/TESTCORE.TST build/TST/TESTPROC.TST=TST/TESTPROC.TST build/TST/TESTRES.TST=TST/TESTRES.TST build/TST/TESTSUP.TST=TST/TESTSUP.TST build/TST/TESTWD.TST=TST/TESTWD.TST build/TST/TESTHB.TST=TST/TESTHB.TST build/TST/TESTLOAD.TST=TST/TESTLOAD.TST build/TST/TESTKEY.TST=TST/TESTKEY.TST build/TST/TESTEVT.TST=TST/TESTEVT.TST build/TST/TESTLOG.TST=TST/TESTLOG.TST build/TST/TESTBOOT.TST=TST/TESTBOOT.TST build/TST/TESTSAFE.TST=TST/TESTSAFE.TST build/TST/TESTPOL.TST=TST/TESTPOL.TST build/TST/TESTHWWD.TST=TST/TESTHWWD.TST build/TST/TESTHLTH.TST=TST/TESTHLTH.TST build/TST/TESTSFP.TST=TST/TESTSFP.TST build/TST/TESTRCV.TST=TST/TESTRCV.TST build/TST/TESTACC.TST=TST/TESTACC.TST build/TST/TESTRCM.TST=TST/TESTRCM.TST build/TST/TESTRST.TST=TST/TESTRST.TST build/TST/TESTLAY.TST=TST/TESTLAY.TST build/TST/TESTINF.TST=TST/TESTINF.TST build/TST/TESTUART.TST=TST/TESTUART.TST build/TST/TESTDATA.TST=TST/TESTDATA.TST build/TST/TESTMB.TST=TST/TESTMB.TST build/TST/TESTSON.TST=TST/TESTSON.TST build/TST/TESTSIM.TST=TST/TESTSIM.TST build/TST/TESTLG60.TST=TST/TESTLG60.TST build/TST/TESTEXE.TST=TST/TESTEXE.TST build/TST/TESTPHY.TST=TST/TESTPHY.TST build/TST/TESTSEQ.TST=TST/TESTSEQ.TST build/TST/TESTIRQ.TST=TST/TESTIRQ.TST build/TST/TESTMTK.TST=TST/TESTMTK.TST build/TST/TESTMTL.TST=TST/TESTMTL.TST build/TST/TESTSONK.TST=TST/TESTSONK.TST build/TST/TESTSONL.TST=TST/TESTSONL.TST build/TST/TESTSONM.TST=TST/TESTSONM.TST build/TST/TESTMTN.TST=TST/TESTMTN.TST build/TST/TESTSONN.TST=TST/TESTSONN.TST build/TST/TESTSONO.TST=TST/TESTSONO.TST build/TST/TESTSONP.TST=TST/TESTSONP.TST build/TST/TESTRXP.TST=TST/TESTRXP.TST build/TST/TESTLGR.TST=TST/TESTLGR.TST build/TST/TESTMRT.TST=TST/TESTMRT.TST build/TST/TESTVWR.TST=TST/TESTVWR.TST build/TST/ACCEPT.TXT=TST/ACCEPT.TXT build/SONARTST.EXE=SONARTST.EXE build/UARTRX.EXE=UARTRX.EXE build/UARTRXT.EXE=UARTRXT.EXE build/SONARDRV.EXE=SONARDRV.EXE build/SONARLOG.EXE=SONARLOG.EXE build/SONARPUB.EXE=SONARPUB.EXE build/SONARVWR.EXE=SONARVWR.EXE build/HBEATOK.EXE=HBEATOK.EXE build/HANGWD.EXE=HANGWD.EXE build/RCONCE.EXE=RCONCE.EXE build/LEAKFD.EXE=LEAKFD.EXE $EXECMT_FILES build/HELLO.EXE=BIN/HELLO.EXE build/NETDRV.EXE=BIN/NETDRV.EXE build/NET.CFG=DOC/NET.CFG

test -s build/disk.img || { echo "ERROR: mkfat16 finished without creating build/disk.img" >&2; exit 1; }
echo "[FAT16] disk.img created: `wc -c < build/disk.img | tr -d ' '` bytes"

# FIX53: mkfat16 created the dynamically sized FAT16 image starting at FAT_LBA.
IMAGE_BYTES=`wc -c < build/disk.img | tr -d ' '`
if [ $((IMAGE_BYTES % 512)) -ne 0 ]; then echo "ERROR: image is not sector aligned" >&2; exit 1; fi
IMAGE_SECTORS=$(( IMAGE_BYTES / 512 ))
FAT_SECTORS=$(( IMAGE_SECTORS - FAT_LBA ))
if [ "$FAT_SECTORS" -le 0 ]; then echo "ERROR: invalid dynamic FAT16 size" >&2; exit 1; fi

# Build the single-source LAY1 descriptor into the first 64 bytes of the
# layout-loader template. Descriptor checksum covers those 64 bytes.
./build/mklayout.exe build/layout_loader.bin build/layout.bin \
    "$KERNEL_LBA" "$KERNEL_BYTES" "$KERNEL_SECTORS" "$KERNEL_RESERVE" \
    "$ACTUAL_GAP" "$FAT_LBA" "$FAT_SECTORS" "$IMAGE_SECTORS" \
    "$FAT_ALIGN_SECTORS" "$FAT_FREE_PERCENT" "$KERNEL_LOAD_ADDR" "$KERNEL_LOAD_LIMIT"

# Overlay only the three non-filesystem disk regions. No hard-coded FAT LBA is
# used here: FAT16 is already at the location computed above.
dd if=build/boot.bin of=build/disk.img bs=512 seek=0 count=1 conv=notrunc
dd if=build/layout.bin of=build/disk.img bs=512 seek=1 count=1 conv=notrunc
dd if=build/kernel.bin of=build/disk.img bs=512 seek="$KERNEL_LBA" conv=notrunc
./build/layoutcheck.exe build/disk.img
./check_fix53_layout_variants.sh build build/layout_loader.bin

# FIX60D: publish a verified bootable image immediately after disk assembly
# and the two layout checks. Later regression checks may still fail the build,
# but an already assembled diagnostic image must not disappear.
rm -f build/toy_os.img.tmp
cp build/disk.img build/toy_os.img.tmp
cmp -s build/disk.img build/toy_os.img.tmp || { echo "ERROR: early toy_os.img copy differs from disk.img" >&2; rm -f build/toy_os.img.tmp; exit 1; }
./build/layoutcheck.exe build/toy_os.img.tmp
mv -f build/toy_os.img.tmp build/toy_os.img
test -s build/toy_os.img || { echo "ERROR: verified build/toy_os.img was not published" >&2; exit 1; }
echo "[IMAGE] build/toy_os.img published after disk assembly: `wc -c < build/toy_os.img | tr -d ' '` bytes"

# Verify every shipped FAT16 entry using the descriptor-driven checker.
for qname in TST TST/ACCEPT.TXT TST/TESTCORE.TST TST/TESTPROC.TST TST/TESTRES.TST TST/TESTSUP.TST TST/TESTWD.TST TST/TESTHB.TST TST/TESTLOAD.TST TST/TESTKEY.TST TST/TESTEVT.TST TST/TESTLOG.TST TST/TESTBOOT.TST TST/TESTSAFE.TST TST/TESTPOL.TST TST/TESTHWWD.TST TST/TESTHLTH.TST TST/TESTSFP.TST TST/TESTRCV.TST TST/TESTACC.TST TST/TESTRCM.TST TST/TESTRST.TST TST/TESTLAY.TST TST/TESTINF.TST TST/TESTUART.TST TST/TESTDATA.TST TST/TESTMB.TST TST/TESTSON.TST TST/TESTSIM.TST TST/TESTLG60.TST TST/TESTEXE.TST TST/TESTSEQ.TST TST/TESTIRQ.TST TST/TESTMTK.TST TST/TESTMTL.TST TST/TESTSONK.TST TST/TESTSONL.TST TST/TESTSONM.TST TST/TESTMTN.TST TST/TESTSONN.TST TST/TESTSONO.TST TST/TESTSONP.TST TST/TESTRXP.TST TST/TESTLGR.TST TST/TESTMRT.TST TST/TESTVWR.TST TST/TESTPHY.TST SONARDRV.EXE SONARTST.EXE UARTRX.EXE UARTRXT.EXE SONARLOG.EXE SONARPUB.EXE SONARVWR.EXE SUPERVIS.EXE HBEATOK.EXE HANGWD.EXE RCONCE.EXE LEAKFD.EXE BOOT.BIN HELLO.EXE COMDRV.EXE LOADER.EXE NETDRV.EXE VGADRV.EXE PROCID.EXE ARGVDIAG.EXE EXIT0.EXE EXIT7.EXE FAULTUD.EXE FAULTGP.EXE FAULTPF.EXE RTD.EXE SENSOR.EXE SENSOR1.EXE SENSOR2.EXE SENSOR3.EXE SENSOR4.EXE SENSOR5.EXE SENSOR6.EXE SENSOR7.EXE SENSOR8.EXE DEADLINE.EXE RTDMISS.EXE RTDSKIP.EXE RTTIME.EXE JITTER.EXE SPLASH.RAW NET.CFG NET_MAST.CFG NET_SLV1.CFG NET_SLV2.CFG QPASS.EXE QPORT.EXE QFROMEXE.EXE QSTOP.EXE QNESTED.EXE QFILE.EXE QSIZE.EXE; do
    ./build/fat16check.exe build/disk.img "$qname"
done
if [ -f build/AUTOSTART.SH ]; then
    ./build/fat16check.exe build/disk.img AUTOSTART.SH
fi
# v60: отдельно проверяем вложенные каталоги и файл внутри них.
./build/fat16check.exe build/disk.img BIN
./build/fat16check.exe build/disk.img BIN/HELLO.EXE
./build/fat16check.exe build/disk.img BIN/NETDRV.EXE
./build/fat16check.exe build/disk.img DOC
./build/fat16check.exe build/disk.img DOC/NET.CFG
for i in `seq 1 25`; do
    mtid=`printf '%02d' "$i"`
    ./build/fat16check.exe build/disk.img "MT${mtid}.EXE"
done


# 4. Базовые 10 проверок + независимая тройная проверка.
./check10.sh
./check3.sh
./check13.sh
./check60.sh
./check61.sh
./check62.sh
./check17.sh
./check21.sh
./check22.sh
./check23.sh
./check24.sh
./check25.sh

./check26.sh
./check27.sh
./check28.sh
./check29.sh
./check30.sh
./check31.sh
./check32.sh
./check33.sh
./check34.sh
./check35.sh
./check36.sh
./check37.sh
./check38.sh
./check39.sh
./check44.sh
./check45.sh
./check46.sh
./check57.sh
./check59.sh

./check63.sh
./check64.sh
./check65.sh
./check66.sh

./check67.sh

# v62 architecture regression: path processing must remain in the standalone module.
./check68.sh
./check69.sh
./check70.sh
sh ./check_rtd_stage1.sh
sh ./check_rtd_stage1_fix3.sh
sh ./check_rtd_stage1_fix6.sh
sh ./check_rtd_stage2.sh
sh ./check_rtd_stage3.sh
sh ./check_rtd_stage3_fix1.sh
sh ./check_rtd_stage3_fix2.sh
sh ./check_rtd_stage3_fix3.sh
sh ./check_rtd_stage3_fix4.sh
sh ./check_rtd_stage3_f10.sh
sh ./check_rtd_stage4_1.sh
sh ./check_rtd_stage4_2.sh
sh ./check_rtd_stage4_3.sh
sh ./check_rtd_stage4_4.sh
sh ./check_rtd_stage5_1.sh
sh ./check_rtd_stage5_1_fix1.sh
sh ./check_rtd_stage5_2.sh
./check_rtd_stage5_2_fix1.sh
./check_rtd_stage5_2_fix2.sh
./check_rtd_stage5_3.sh
sh ./check_rtd_stage6_1.sh
sh ./check_rtd_stage6_1_fix1.sh
sh ./check_rtd_stage6_1_fix2.sh
sh ./check_rtd_stage6_1_fix3.sh
./check71.sh
./check72.sh
./check73.sh
./check74.sh
./check75.sh
./check76.sh
./check77.sh
./check78.sh
./check79.sh
./check80.sh
./check81.sh
./check82.sh
./check83.sh
./check84.sh
./check85.sh
./check88.sh
./check86.sh
./check87.sh
./check89.sh
./check90.sh
./check91.sh
./check92.sh
./check99.sh
./check100.sh
./check101.sh
./check102.sh
./check_fix53_dynamic_layout.sh

# FIX60B: all current architectural acceptance checks must be part of the build.
./check_fix54_infrastructure.sh
./check_fix55_uart.sh
./check_fix56_data_channel.sh
./check_fix57_modbus.sh
./check_fix58_sonar.sh
./check_fix59_sonarsim.sh
./check_fix60_telemetry.sh
./check_fix60a_executable_loader.sh
./check_fix60b_build_finalization.sh
./check_fix60c_fat83_names.sh
./check_fix60d_image_publication.sh
./check_fix60e_uart_rx.sh
./check_fix60f_uart_irq.sh
./check_fix60g_uart_rx_poll.sh
./check_fix60h_uart_full_poll.sh
./check_fix60i_sonar_repeat.sh
./check_fix60j_mt_console_return.sh
./check_fix60k_execmt_commit.sh
./check_fix60l_console_ownership.sh
./check_fix60m_sonar_no_yield.sh
./check_fix60n_execmt_scheduler.sh
./check_fix60o_execmt_serial_boundary.sh
./check_fix60p_uart_rx_isolation.sh
./check_fix60q_qemu_socket_boundary.sh
./check_fix60r_sonarlog_lifecycle.sh
./check_fix60s_sonarlog_endurance.sh
./check_fix60t_sonarview.sh

# FIX60D: FINAL ARTIFACT GATE. A successful build is impossible without a
# descriptor-valid disk.img and a byte-identical, non-empty toy_os.img.
test -s build/disk.img || { echo "ERROR: final build/disk.img is missing or empty" >&2; exit 1; }
./build/layoutcheck.exe build/disk.img
rm -f build/toy_os.img.final
cp build/disk.img build/toy_os.img.final
test -s build/toy_os.img.final || { echo "ERROR: final image staging copy was not created" >&2; exit 1; }
cmp -s build/disk.img build/toy_os.img.final || { echo "ERROR: final staged image differs from verified build/disk.img" >&2; rm -f build/toy_os.img.final; exit 1; }
./build/layoutcheck.exe build/toy_os.img.final
mv -f build/toy_os.img.final build/toy_os.img
test -s build/toy_os.img || { echo "ERROR: final build/toy_os.img was not published" >&2; exit 1; }
cmp -s build/disk.img build/toy_os.img || { echo "ERROR: published build/toy_os.img differs from verified build/disk.img" >&2; exit 1; }
./build/layoutcheck.exe build/toy_os.img
./build/fat16check.exe build/toy_os.img SONARDRV.EXE >/dev/null
./build/fat16check.exe build/toy_os.img TST/TESTEXE.TST >/dev/null
./build/fat16check.exe build/toy_os.img TST/TESTSEQ.TST >/dev/null
./build/fat16check.exe build/toy_os.img TST/TESTMTN.TST >/dev/null
./build/fat16check.exe build/toy_os.img TST/TESTSONN.TST >/dev/null
./build/fat16check.exe build/toy_os.img TST/TESTSONO.TST >/dev/null
./build/fat16check.exe build/toy_os.img TST/TESTSONP.TST >/dev/null
./build/fat16check.exe build/toy_os.img TST/TESTRXP.TST >/dev/null
./build/fat16check.exe build/toy_os.img TST/TESTLGR.TST >/dev/null
./build/fat16check.exe build/toy_os.img SONARTST.EXE >/dev/null
./build/fat16check.exe build/toy_os.img SONARPUB.EXE >/dev/null
./build/fat16check.exe build/toy_os.img UARTRX.EXE >/dev/null
./build/fat16check.exe build/toy_os.img UARTRXT.EXE >/dev/null
FINAL_IMAGE_BYTES=`wc -c < build/toy_os.img | tr -d ' '`
echo "[FINAL] build/toy_os.img created and verified: ${FINAL_IMAGE_BYTES} bytes"
echo "BUILD AND VERIFICATION OK"
