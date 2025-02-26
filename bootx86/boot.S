
#include "symbol.h"
#include "boot.h"
.code16         

.text
FUNCTION(_start)
    cli                 
    xorw %ax, %ax       
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    movw $0x7c00, %sp

    sti
    movw $msg, %si
    call LOCAL(print_string)

    hlt

LOCAL(print_string):
    lodsb                
    orb %al, %al         
    jz done              
    movb $0x0e, %ah     
    int $0x10           
    jmp LOCAL(print_string)
done:
    ret

msg:
    .asciz "Hello, World!"

    .fill 510-(.-_start), 1, 0 
    .word BOOT_BIOS_SIGNATURE 
