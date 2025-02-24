/* we need a signature; 0xaa55 & in this stage 1 we can only use 512 B */


.code16
    .file "bios.S"

#include "symbols.h"
.text
FUNCTION(read_disk_sector)
    movb $0x02, %ah        # Read function
    movb $0x01, %al        # Read 1 sector
    movb $0x00, %ch        # Cylinder 0
    movb $0x02, %cl        # Sector 2
    movb $0x00, %dh        # Head 0
    movb $0x80, %dl        # First hard disk
    movw $0x7e00, %bx      # Buffer location
    movw %bx, %es
    int $0x13              # Call BIOS disk service


