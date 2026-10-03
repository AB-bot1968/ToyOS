SECTIONS
{
    . = 0x0600;
    .text : { *(.text*) *(.data*) }
}
