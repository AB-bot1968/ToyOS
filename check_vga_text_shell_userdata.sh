#!/bin/sh
set -eu
ok(){ echo "PASS: $1"; }
bad(){ echo "FAIL: $1"; exit 1; }

# The F10/RT preparation state is mutable Ring-3 shell data. It must not stay in
# the kernel .bss, because the normal page table intentionally keeps that area
# supervisor-only.
grep -q '__attribute__((section(".userdata"),used)) static char pending_rt_args' src/user_shell.c && ok 'pending RT queue is in .userdata' || bad 'pending RT queue still in kernel .bss'
grep -q '__attribute__((section(".userdata"),used)) static uint32_t pending_rt_count=0u' src/user_shell.c && ok 'pending RT count is in .userdata' || bad 'pending RT count is not in .userdata'
grep -q '__attribute__((section(".userdata"),used)) static uint32_t shell_f10_event=0u' src/user_shell.c && ok 'F10 event state is in .userdata' || bad 'F10 event state is not in .userdata'
grep -q '\.userdata' liker.ld && ok 'linker has .userdata output section' || bad 'linker .userdata output section missing'
grep -q '__user_data_start' src/kernel.c && grep -q '__user_data_end' src/kernel.c && ok 'kernel knows user-data bounds' || bad 'kernel user-data bounds missing'
grep -q 'paging_mark_user_rw((uint32_t)__user_data_start,(uint32_t)__user_data_end)' src/kernel.c && ok 'main page table maps .userdata as user-writable' || bad 'user data is not mapped writable'
grep -q -- '--only-section=.udata' build.sh && ok 'kernel image includes .udata' || bad 'kernel image omits .udata'

# Compile the shell object and verify the compiler emitted a dedicated userdata
# section rather than ordinary .bss.
gcc -m32 -Os -ffreestanding -fno-pie -fno-stack-protector -fno-asynchronous-unwind-tables -fno-unwind-tables -fno-builtin -fno-tree-vectorize -fno-tree-slp-vectorize -mno-sse -mno-sse2 -mno-mmx -mno-80387 -nostdinc -nostdlib -c src/user_shell.c -o /tmp/toyos_user_shell_userdata_check.o
readelf -S /tmp/toyos_user_shell_userdata_check.o | grep -q '\.userdata' && ok 'compiler emitted .userdata' || bad 'compiler did not emit .userdata'

# Link the kernel+shell layout with the project linker script and verify the
# user-data range is page-aligned and is not the kernel .bss.
gcc -m32 -Os -ffreestanding -fno-pie -fno-stack-protector -fno-asynchronous-unwind-tables -fno-unwind-tables -fno-builtin -fno-tree-vectorize -fno-tree-slp-vectorize -mno-sse -mno-sse2 -mno-mmx -mno-80387 -nostdinc -nostdlib -c src/kernel.c -o /tmp/toyos_kernel_userdata_check.o
ld -m elf_i386 -r -T liker.ld -o /tmp/toyos_userdata_layout_check.o /tmp/toyos_kernel_userdata_check.o /tmp/toyos_user_shell_userdata_check.o
readelf -S /tmp/toyos_userdata_layout_check.o | grep -q '\.udata' && ok 'linked image contains .udata' || bad 'linked image omits .udata'
nm -n /tmp/toyos_userdata_layout_check.o | grep -q '___user_data_start' && nm -n /tmp/toyos_userdata_layout_check.o | grep -q '___user_data_end' && ok 'linked user-data boundary symbols exist' || bad 'linked user-data boundary symbols missing'

echo 'VGA/text shell user-data checks passed'
