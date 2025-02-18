#include <stdio.h>
#include <stdlib.h>

#ifndef TAGS_H
#define TAGS_H 1

/*
I want to reuse this header file outside this project
*/
#define TAGS_API extern __attribute__((visibility("default")))

#define TAG_VERSION_NUMBER 1000000
#define VERSION "1.0"

// not exactly stable, can be changed any time
#define VOLATILE_API

#define MAX_TAGS 10

#define assert_nq(v, k, rxn)                                                  \
	do {                                                                  \
		if (v > k)                                                    \
			fprintf(stderr, "%s:%d %s", __FILE__, __LINE__, rxn); \
	} while (0);

enum {
	ERROR = 1,
	MISUSE,
	NOMEM,
};
struct FileMetadata {
	char **tags;
	char *filepath;
	size_t file_size;
	size_t rows;
	struct FileMetadata *next;
};

#define DEBUG 1
/* tag unstable routines*/

VOLATILE_API void deleteFileFromListByFilename(const char *);

// write to file temp file
VOLATILE_API void showFileTags();
VOLATILE_API void saveFilesToList(struct FileMetadata **fp, char *filename);

TAGS_API void *copyOverSharedMem(
    char *src,
    /* src to mmap*/ char *dst /* file dst, in our pwd*/);
TAGS_API char *generateTempJsonFile();
TAGS_API struct FileMetadata *createFileTags(const char *filename,
					     int tags,
					     ...);

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

#endif /*tags */
#include <string.h>
static void usage(char *progname, FILE *usage_stream_buffer) {
	fprintf(usage_stream_buffer,
		"%s - commandline FileTag [version %s]\n"
		"\nUsage:\t%s [options] <File Tag> [file...]\n"
		"\t%s [options] -a <add a file tag> [strings...]\n"
		"\t%s [options] -r <remove a file tag> [strings...]\n"
		"\t%s [options] -s <search a file> [tag/file [[string]]\n\n"
		"File tagging is commandline tool    for adding tags to files\n"

		"Example:\n\n\t$ ./build/tag -a write-tags <tags>\n",
		progname, VERSION, progname, progname, progname, progname);
}

#endif /*TAGS_H*/
