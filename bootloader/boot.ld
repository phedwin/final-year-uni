

ENTRY(__boot)

SECTIONS {
    . = 0x7c00;
    __boot_start = .;

    .boot :
    {
        *(.boot .boot.*)
    }
    .text :
    {
        *(.text .text.*)
    }
    .rodata :
    {
        *(.rodata .rodata.*)
    }
    .data :
    {
        *(.data .data.*)
        *(.got .got.*)
    }
    __boot_end = .;

    /* Partition table (MBR) */
    . = 0x7c00 + 446;
    _partition_table = .;
    .partition_table :
    {
        QUAD(0) QUAD(0)   /* Entry 0 */
        QUAD(0) QUAD(0)   /* Entry 1 */
        QUAD(0) QUAD(0)   /* Entry 2 */
        QUAD(0) QUAD(0)   /* Entry 3 */
    }

    /* Boot signature */
    . = 0x7c00 + 510;
    .magic_number :
    {
        SHORT(0xaa55)
    }
}

