

#ifndef COMMON_H

#define COMMON_H 1

#define _POSIX_C_SOURCE 200809L

#if defined(SOLARIS) /* Solaris 10 */
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 700
#endif

/*symbols*/
#define FUNCTION(x)                \
	.globl EXT_C(x);           \
	.type EXT_C(x), @function; \
	EXT_C(x) :
#define VARIABLE(x)              \
	.globl EXT_C(x);         \
	.type EXT_C(x), @object; \
	EXT_C(x) :

#include <sys/termios.h> /* for winsize */
#include <sys/types.h>   /* some systems still require this */
#if defined(MACOS) || !defined(TIOCGWINSZ)
#include <sys/ioctl.h>
#endif

#include <signal.h> /* for SIG_ERR */
#include <stddef.h> /* for offsetof */
#include <stdio.h>  /* for convenience */
#include <stdlib.h> /* for convenience */
#include <unistd.h> /* for convenience */

#define MAXLINE 4096 /* max line length */

/*
 * Default file access permissions for new files.
 */
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

/*
 * Default permissions for new directories.
 */
#define DIR_MODE (FILE_MODE | S_IXUSR | S_IXGRP | S_IXOTH)

#define MODE (O_RDWR | O_CREAT | O_APPEND)

typedef void Sigfunc(int); /* for signal handlers */

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

#define PAGE_BUFFER sysconf(_SC_PAGE_SIZE)
#define BUFFER (PAGE_BUFFER / 4)

#define DEBUG 1

#include <assert.h> /*EGDE CASES*/
#include <string.h> /* STRING OPERATIONS*/
#include <sys/stat.h>

#define COMMON_TODO(rxn)                                                      \
	do {                                                                  \
		fprintf(stdout, "\e[31m%s %s:%d\n", rxn, __FILE__, __LINE__); \
	} while (0);
/* FOR TAGS*/
#define TAG_VERSION_NUMBER 1000000
#define VERSION "1.0"

static int TAG_USAGE(const char *progname, FILE *usage_stream_buffer) {
	fprintf(
	    usage_stream_buffer,
	    "%s - commandline FileTag [version %s]\n"
	    "\nUsage:\t%s [options] <File Tag> [file...]\n"
	    "\t%s [options] -a <FILE(changes are written here)> <add a file "
	    "tag> [strings...]\n"
	    "\t%s [options] -r <remove a file tag> [strings...]\n"
	    "\t%s [options] -s <search a file> [tag/file [[string]]\n\n"
	    "File tagging is commandline tool    for adding tags to files\n"

	    "Example:\n\n\t$ ./build/tag -a write-tags <tags>\n",
	    progname, VERSION, progname, progname, progname, progname);
	return EXIT_FAILURE;
}

#ifdef TAGS
// General Use Tags:
static char *generalFileTags[] = {
	"important",   "urgent",    "draft",  "final",   "completed",
	"in-progress", " archived", "review", "pending", "deleted",
};

// Project-Based Tags:
static char *generalProjectTags[] = {
	"frontend",
	"docs",
	"assets",
	"prototype",
};

#endif /*tags pg 28 */

#endif /* ! COMMON_H*/