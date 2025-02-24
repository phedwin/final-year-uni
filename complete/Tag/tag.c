#include <stdio.h>
#include <string.h>
#include "common.h"

/*
 *- close fd(s)
 *- free memory
 *- sync file writings
 *- set stream buffers & fflush
 *- Do not overwrite filetags
 **/

/*HOW WE ARE LAID IN MEMORY */
struct File {
	char **tags;       /* Array of Tags */
	char *filename;    /* File we are tagging*/
	size_t count;      /* Number of tags*/
	size_t rows;       /* keep count of lines for seek & append*/
	size_t length;     /*preserve NR*/
	struct File *next; /*ptr to next entry*/
};
/*List Entry*/
static struct File *head = 0;

/*routines*/
void *xmalloc(size_t);
void *xrealloc(void *, size_t);
void fatal(char *);

/*progname*/
const char *progname = "phedwin";

/*store file details alongside tags, *prettified in this file*/
char *generateTempTagFile();
void addTagsToTheFile(char *filename, char **tags, int count);
void persistToFile(int fd, char *buffer, size_t tags_cnt);

#include <bits/getopt_core.h>
#include <fcntl.h>
#include <sys/mman.h>

void *copyOverSharedMem(int source, int dst);
#include <unistd.h>

#define SHORTOPTS "a:s:d:"

void syncTagsToFile(char *path, size_t tags_t, char *buffer, char **tags) {
	FILE *stream = fopen(path, "w+");

	if (stream == 0)
		return;
	fprintf(stream, "| NR  | FILE  | TAGS                 | COUNT |\n");
	fprintf(stream, "|--- | ----- | -------------------- | ----- |\n");

	struct File *current = head;
	char buf[80];

	/*Members  */
	while (current) {
		for (int k = 0; k < tags_t; k++) {
			snprintf(buf, sizeof(char *) * tags_t, " %s ", tags[k]);
			strcat(buffer, buf);
		}
		fprintf(stream, "| %zu | %s      | %s  | %zu |\n",
			current->length, path, buffer, tags_t);
		current->length++;
		current = current->next;
	}
	fclose(stream);
}

#define assert_eq(logic, rxn)                \
	do {                                 \
		if (logic)                   \
			;                    \
		else {                       \
			printf("%s\n", rxn); \
			EXIT_FAILURE;        \
		}                            \
	} while (0);
#include <stdarg.h>
/* the stdarg to help make a choice btwn searching by filaname or searching by
 * tags, the idea was, i dont get to write different routines for the same
 * functionality, this routine responds to flag -s*/

/*TODO, rewrite.*/
void *genericSearch(char **buf /*keep results of tags, or files sharing tags*/,
		    char *filename /*keep this 0*/,
		    char **tags /*or this*/,
		    int category,
		    ...) {
	struct File *attr = head;
	int index = 0;
	char *choice = 0;
	va_list list;

	assert_eq(category == 1, "you cannot have more than one entries");
	va_start(list, category);
	for (int m = 0; m < category; m++)
		choice = va_arg(list, char *);
	va_end(list);

	/*search by filaname*/
	if (strcmp(choice, "filename")) {
		while (attr) {
			if (strcmp(attr->filename, filename) == 0) {
				*(buf + index) = attr->filename;
				index++;
			}
			attr = attr->next;
		}
	} else if (strcmp(choice, "tag")) {
		while (attr) {
			for (int q = 0; tags[q] != NULL; q++)
				*(buf + index) = tags[q];
			attr = attr->next;
		}

	} else {
		printf("unsupported choice: please use filename or tag");
	}

	return buf;
}

/*routine to clean everything, tags & files, The core implementation expands in
 * main. And also it removes the file we were writing changes to, this routine
 * responds to the -d flag */
void destroyListEntries(char *path) {
	struct File *entries = head;
	while (entries) {
		free(entries);
		entries = entries->next;
	}
	remove(path);
}
int main(int argc, char **argv) {
	if (argc < 3)
		progname == NULL ? TAG_USAGE(argv[0], stderr)
				 : TAG_USAGE(progname, stderr);

	char *filename = 0, *path = 0;
	int opts;
	while ((opts = getopt(argc, argv, SHORTOPTS)) != -1) {
		switch (opts) {
				/*case 'a' usage is bin f -a [FILE]
				 *[....TAGS] f is file u want to write
				 *to*/
			case 'a': {
				char *tagged_file = optarg;
				/*we search, duplcate, add, remove from
				 * this*/
				filename = argv[1];
				/* TempFile Generated, we decided with a
				 * markdown, its easy to write and
				 * parse, no extra work like parsing
				 * imagining if we had either json or
				 * toml, which were better choices */
				path = generateTempTagFile();
				if (path == 0)
					return EXIT_FAILURE;

				/*the idea is if we get the flag option
				 * for add and then we need the extra
				 * filepath to write the changes u made,
				 * in that we have the fd, & also that;
				 * i thought open was better & maybe we
				 * can move to fopen */
				int fd_src, fd_dst;

				fd_src = open(path, MODE, FILE_MODE);
				fd_dst = open(filename, MODE, FILE_MODE);

				if (fd_src < 0 || fd_dst < 0)
					return EXIT_FAILURE;

				// keeping everything under 4K
#define BUF MAXLINE
				char buf[BUF];
				/*Tags are added to the temp file first,
				 * This is the idea i had, something
				 * like journal, so we can always have a
				 * good failover and incase it fails to
				 * write then the user file is safe
				 * (this is lowkey stupid now that im
				 * thinking about it ) its not like its
				 * going to fail*/
				addTagsToTheFile(tagged_file, argv, argc);

				/*writing to the temporary file,
				 * autogenerated*/
				syncTagsToFile(path, argc, buf, argv);

				/* if the write was successful, save to
				 * disk,
				 * TODO sync write changes, close
				 * buffers and setvbuf to line buf
				 * setvbuf(stream, buf, _IOLBF, BUF); */

				void *mem = copyOverSharedMem(fd_src, fd_dst);
				/* release mapped mem*/
				munmap(mem, 4096);
				/*unlink the journal*/
				remove(path);
				/*success copy to disk ? */
				if (fsync(fd_dst) < 0)
					return errno;

				/* clean everything*/
				close(fd_dst);
				close(fd_src);
				free(tagged_file);
			} break;
				/*search tags  */

				/*bin tags/file -s [tags/file] */
			case 's': {
				char *buffer[argc];
				char *buf = genericSearch(buffer, filename,
							  argv, 1, argv[1]);
			} break;

				/*bin -d [file]*/
			case 'd':
				destroyListEntries(optarg);

				break;
			case '?':
				COMMON_TODO("unimplemented!")
				break;
		}
	}

	free(path);
	return 0;
}

struct File *createNewFileTags(char **tags, int count, char *path) {
	struct File *attributes = xmalloc(sizeof(struct File));

#define TAG_ALLOC_SIZE sizeof(char *) * count
	assert(tags[count - 1] != NULL);
	attributes->tags = xmalloc(TAG_ALLOC_SIZE);
	for (int k = 0; k < count; k++) {
		if (attributes->tags == 0)
			attributes->tags =
			    xrealloc(attributes->tags, TAG_ALLOC_SIZE * 2);
		attributes->tags[k] = tags[k];
	}
	attributes->filename = path;
	attributes->count = count;
	attributes->length = 0;

	return attributes;
}
void addTagsToTheFile(char *filename, char **tags, int count) {
	struct File *new = createNewFileTags(tags, count, filename);
	new->next = head;
	head = new;
}

static char filepath[32] = "/tmp/file-tags-store-XXXXXX";

#include <unistd.h>

/*
* this looks stupid, but its like our journal, incase it *fails
* somewhere along the execution, we dont proceed to write to file
specified by
 * user. Its not overcooked. Just the right amount of fault torelation.
 * */
char *generateTempTagFile() {
	int cx = 0;
	if ((cx = mkstemp(filepath)) < 0)
		return 0; /*NULL*/

	/* ok we dont need the generated file, but we will use the path
	 * to make an ext tag file */
	unlink(filepath);
	close(cx);
	char *buf = malloc(sizeof filepath);
	cx = snprintf(buf, sizeof filepath, "%s%s", filepath, ".tag");
	if (cx < 0 && cx >= sizeof filepath)
		goto CLEAN;
	return buf;
CLEAN:
	free(buf);
	return 0;
}

void fatal(char *reason) {
	fprintf(stderr, "Fatal: %s on %d:%s", reason, __LINE__, __FILE__);
}

void *xmalloc(size_t size) {
	void *ptr;
	if ((ptr = malloc(size)) == 0)
		fatal("exhausted virtual mem");
	return ptr;
}

void *xrealloc(void *ptr, size_t size) {
	void *new_ptr;
	if ((new_ptr = realloc(ptr, size)) == 0)
		fatal("exhausted virtual mem");
	return new_ptr;
}

#ifdef DEBUG
void *copyOverSharedMem(int source, int dst) {
	void *addrs;

	struct stat stats;
	memset(&stats, 0, sizeof(struct stat));

	if (fstat(source, &stats) < 0)
		return 0;
#define source_file_size stats.st_size
	addrs = mmap(0, (size_t)source_file_size, PROT_WRITE, MAP_SHARED,
		     source, 0);

	if (addrs == MAP_FAILED)
		return 0;
	if (ftruncate(dst, source_file_size) < 0)
		return 0;

	size_t bytes_written;
	if ((bytes_written = write(dst, addrs, stats.st_size)) <
	    stats.st_size) {
		fprintf(stderr, "failed on partial write\n");
		return 0;
	}
	return addrs;
}
#endif
