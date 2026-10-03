/* Link a standalone Toy OS EXE at the address where the kernel loads it. */
SECTIONS
{
    . = 0x00100000;
    .text :
    {
        KEEP(*(.start))
        *(.text*)
        *(.usertext*)
        *(.usercode_data*)
        *(.rodata*)
        *(.rdata*)
        *(.data*)
    }
    __program_bss_start = .;
    .bss (NOLOAD) :
    {
        *(.bss*)
        *(COMMON)
    }
    __program_bss_end = .;
}
