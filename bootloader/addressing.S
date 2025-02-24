
/* LEARN */

.file addressing.S
#include "symbols.h"
.data
    string: .ascii "word"
.text

FUNCTION(STR_LENGTH)
    xor %esi, %esi  /* this should be our index */
1:
    movb string(, %esi, 1), %al /* al store the lower 8 bits of the string until it hits 0 */
    cmpb $0, %al
    jz .2
    inc %esi
    jmp 1b
.2:
    mov %esi, %eax
    ret

/*

** program that counts how many times the character `'a'` appears in a null-terminated string.**

For the string `"banana"`, the output should be `3`.

1. Store the string in `.data` as `.ascii` or `.string`.  
2. Use `%esi` as an index and `%al` to hold the current character.  
3. Compare `%al` with `'a'` and increment a counter if they match.  
4. Stop when you hit a null byte (`0`).  
5. Store the result in `%eax` before returning.

*/



.data
    string: 
        .ascii "banana"
.text
FUNCTION(COUNT_CHARACTERS)
    xor %esi, %esit
    xor %eax, %eax 
count: 
    movb string(, %esi, 1), %al
    cmpb $97, %al /* i did not know what do in first glance so i hardcoded 'a' turns out u can $'a' */
    jz .increment
    inc %esi
    testb %al, %al
    jz .done
    jmp count
.increment:
    inc %eax
    jmp count
.done: 
    ret



