.text
#include "symbols.h"

FUNCTION(_start)
    mov $60, %rax       # syscall number for exit
    xor %rdi, %rdi      # exit code 0
    syscall

