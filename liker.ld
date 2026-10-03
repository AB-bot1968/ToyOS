/* PE/COFF linker script for the 32-bit Toy OS.
   Kernel text/data are supervisor-only; the ring-3 shell is aligned to page
   boundaries so paging can grant user access only to its code/rodata. */
SECTIONS
{
    . = 0x7e00;
    .text :
    {
        KEEP(*(.start))
        *(.text*)
        *(.krodata*)
        *(.data*)
    }

    . = ALIGN(0x1000);
    ___user_text_start = .;
    .utext :
    {
        *(.usertext*)
    }
    ___user_text_end = .;

    . = ALIGN(0x1000);
    ___user_rodata_start = .;
    .urodata :
    {
        *(.rdata*)
        *(.rodata*)
    }
    ___user_rodata_end = .;

    . = ALIGN(0x1000);
    ___user_data_start = .;
    .udata :
    {
        *(.userdata*)
    }
    ___user_data_end = .;

    ___bss_start = .;
    .bss (NOLOAD) :
    {
        *(.bss*)
        *(COMMON)
    }
    ___bss_end = .;
}
