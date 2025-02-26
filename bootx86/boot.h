
#ifndef BOOT_HEADER
#define BOOT_HEADER 1

/* The signature for bootloader, 512B on MBR  */
#define BOOT_BIOS_SIGNATURE 0xaa55

/* The offset of the start of BPB (BIOS Parameter Block).  */
#define BOOT_BPB_START 0x3

/* The offset of the end of BPB (BIOS Parameter Block).  */
#define GRUB_BOOT_MACHINE_BPB_END 0x5a

/* The offset of KERNEL_SECTOR.  */
#define BOOT_MACHINE_KERNEL_SECTOR 0x5c

/* where in memory we load the MBR */
#define BOOT_STACK_SEGMENT 0x7c00

#endif /* ! BOOT_HEADER */
