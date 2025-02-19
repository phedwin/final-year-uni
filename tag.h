#include "common.h"

struct File {
	char **tags;          /* Array of Tags */
	const char *filename; /* File we are tagging*/
	struct File *next;
	size_t count; /* Number of tags*/
	size_t rows;  /* keep count of lines for seek & append*/
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
void addTagsToTheFile(const char *filename, char **tags, int count);
void persistToFile(int fd, char *buffer, size_t size, size_t tags_cnt);

#include <bits/getopt_core.h>
#include <fcntl.h>
#include <sys/mman.h>

void *copyOverSharedMem(int fd_src, int fd_dst);
#include <unistd.h>

#define SHORTOPTS "a:"

int main(int argc, char **argv) {
	if (argc < 3)
		progname == NULL ? TAG_USAGE(argv[0], stderr)
				 : TAG_USAGE(progname, stderr);
	int opts;
	char *filename = 0, *path = 0;
	while ((opts = getopt(argc, argv, SHORTOPTS)) != -1) {
		switch (opts) {
			case 'a': {
				filename = optarg;
				path = generateTempTagFile();

				int fd_src, fd_dst;
				fd_src =
				    open(path /*journal*/, MODE, FILE_MODE);
				fd_dst = open(filename /*user -a option*/, MODE,
					      FILE_MODE);

				struct stat stats;
				if (stat(path, &stats) < 0)
					return EXIT_FAILURE;

				addTagsToTheFile(path /*journal*/, argv, argc);
				char buf[stats.st_size];
				persistToFile(fd_src, buf, sizeof buf, argc);

				/* if the write was successful, save to disk*/
				copyOverSharedMem(fd_dst, fd_src);
				/*unlink the journal*/
				unlink(path);
			} break;

			case '?':
				COMMON_TODO("unimplemented!")
				break;
		}
	}

	free(path);
}

struct File *createNewFileTags(char **tags, int count, const char *path) {
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

	return attributes;
}
void addTagsToTheFile(const char *filename, char **tags, int count) {
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
	// close the fd & make a new filename, with extension & maybe
	// unlink
	close(cx);
	char *buf = malloc(sizeof filepath * 2);
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
	exit(EXIT_FAILURE);
}

void *xmalloc(size_t size) {
	void *ptr;
	if ((ptr = malloc(size)) == 0)
		fatal("exhausted virtual mem");
	return ptr;
}

void *xrealloc(void *ptr, size_t size) {
	void *new_ptr;
	if ((new_ptr = realloc(ptr, size)) == 0) {
		fatal("exhausted virtual mem");
		exit(EXIT_FAILURE);
	}
	return new_ptr;
}

#ifdef DEBUG
void *copyOverSharedMem(int fd_src
			/* src to mmap*/,
			int fd_dst /* file dst, in our pwd*/) {
	void *addr;

	struct stat stats = { 0 };

	if (fstat(fd_src, &stats) < 0)
		goto failed;
#define SRC_FILE_SIZE stats.st_size
	addr = mmap(NULL, SRC_FILE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
		    fd_src, 0);

	if (addr == MAP_FAILED)
		goto failed;
	if (fd_src < 0 || fd_src < 0)
		goto failed;

	FILE *stream = fdopen(fd_src, "r");

	write(fd_dst, addr, SRC_FILE_SIZE);
	fclose(stream);
	/* return for munmap*/
	return addr;
failed:
	return (void *)-1;
}
#endif

void persistToFile(int fd, char *buffer, size_t size, size_t tags_cnt) {
	struct File *current = head;

	/*assign to make it a little cleaner & readable*/
	char *path = 0, **tags = 0;
	size_t count = 0, lines = 0;
	FILE *stream = fdopen(fd, "w+");

	char *t = "| FILENAME        |           TAGS             |TagsCount |";
	char *h = "|---------------------------------------------------------|";
	char *e = "------------------|----------------------------|-----------";
	fprintf(stream, "%s\n%s\n", t, h);
	do {
		count = current->count;
		lines = current->rows;
		path = strdup(current->filename);

		snprintf(buffer, size,
			 "| %s                |         %s    |      %zu   | "
			 "%zu |\n",
			 path, current->tags[0] /*TODO*/, count, lines);

		fprintf(stream, "%s\n", buffer);
		current = current->next;
	} while (current);

	fprintf(stream, "%s\n", e);
	return;
clean:
	free(path);
	fclose(stream);
}