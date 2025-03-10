#undef _LARGEFILE_SOURCE
#undef _FILE_OFFSET_BITS
#define _LARGEFILE_SOURCE
#define _FILE_OFFSET_BITS 64
#if defined(__PPC__) && !defined(__powerpc__)
#define __powerpc__ 1
#endif

#define GCRYPT_NO_DEPRECATED 1
#define HAVE_MEMMOVE 1

/* Define to 1 to enable disk cache statistics.  */
#define DISK_CACHE_STATS @DISK_CACHE_STATS @
#define BOOT_TIME_STATS @BOOT_TIME_STATS @

/* We don't need those.  */
#define MINILZO_CFG_SKIP_LZO_PTR 1
#define MINILZO_CFG_SKIP_LZO_UTIL 1
#define MINILZO_CFG_SKIP_LZO_STRING 1
#define MINILZO_CFG_SKIP_LZO_INIT 1
#define MINILZO_CFG_SKIP_LZO1X_1_COMPRESS 1
#define MINILZO_CFG_SKIP_LZO1X_DECOMPRESS 1

#if defined(GRUB_BUILD)
#undef ENABLE_NLS
#define BUILD_SIZEOF_LONG @BUILD_SIZEOF_LONG @
#define BUILD_SIZEOF_VOID_P @BUILD_SIZEOF_VOID_P @
#if defined __APPLE__
#if defined __BIG_ENDIAN__
#define BUILD_WORDS_BIGENDIAN 1
#else
#define BUILD_WORDS_BIGENDIAN 0
#endif
#else
#define BUILD_WORDS_BIGENDIAN @BUILD_WORDS_BIGENDIAN @
#endif
#elif defined(GRUB_UTIL) || !defined(GRUB_MACHINE)
// #include <config-util.h>
#else
#define HAVE_FONT_SOURCE @HAVE_FONT_SOURCE @
/* Define if C symbols get an underscore after compilation. */
#define HAVE_ASM_USCORE @HAVE_ASM_USCORE @
/* Define it to one of __bss_start, edata and _edata.  */
#define BSS_START_SYMBOL @BSS_START_SYMBOL @
/* Define it to either end or _end.  */
#define END_SYMBOL @END_SYMBOL @
/* Name of package.  */
#define PACKAGE "@PACKAGE@"
/* Version number of package.  */
#define VERSION "@VERSION@"
/* Define to the full name and version of this package. */
#define PACKAGE_STRING "@PACKAGE_STRING@"
/* Define to the version of this package. */
#define PACKAGE_VERSION "@PACKAGE_VERSION@"
/* Define to the full name of this package. */
#define PACKAGE_NAME "@PACKAGE_NAME@"
/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "@PACKAGE_BUGREPORT@"

#define GRUB_TARGET_CPU "@GRUB_TARGET_CPU@"
#define GRUB_PLATFORM "@GRUB_PLATFORM@"

#define RE_ENABLE_I18N 1

#define _GNU_SOURCE 1

#endif
