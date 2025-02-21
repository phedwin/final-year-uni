#ifndef ASM_SYM
#define ASM_SYM

#define EXT_C(x) x

/* Function symbol */
#define FUNCTION(x)                \
	.globl EXT_C(x);           \
	.type EXT_C(x), @function; \
	EXT_C(x) :

/* Variable symbol */
#define VARIABLE(x)              \
	.globl EXT_C(x);         \
	.type EXT_C(x), @object; \
	EXT_C(x) :

#endif /* ASM_SYM */
