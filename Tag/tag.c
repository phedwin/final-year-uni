

// TODO HEAP IS EVERYWHERE, CLEAN!
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "tag.h"

#define TAGS 1

struct FileTagOperations {
	struct FileMetadata (*createFileTags)(const char *filename,
					      int argc,
					      ...);
};

// please please rewrite this
// you have enough memory, go crazy.
struct FileMetadata *createFileTags(const char *filename,
				    int argc /*optind*/,
				    ...) {
	struct FileMetadata *new = malloc(sizeof(struct FileMetadata));

	new->filepath = strdup(filename);
	struct stat stats;

	if (stat(filename, &stats) < 0)
		free(new);

	new->file_size = stats.st_size;

	// funfact ur addressing 1 << 48 maybe 54 bit on that 64 bit computer
	// 80% of the application is on virtual memory, u have demand paging
	// memory WILL NEVER FAIL. Source - trust me.
	new->tags = malloc(sizeof(char *) * MAX_TAGS);

	va_list args;

	assert_nq(argc /*v*/, MAX_TAGS /*k*/, "file tag overlap");

	va_start(args, argc);
	for (int arg = 0; arg < argc; arg++)
		// y'know sometimes this things can fail, anyway 🏄
		new->tags[arg] = strdup(va_arg(args, char *));

	va_end(args);
	return new;

failed_tags:
	free(new->filepath);
	free(new);
	exit(EXIT_FAILURE);
}

static struct FileMetadata *head = 0;
void saveFilesToList(struct FileMetadata **fp, char *filename) {
	struct FileMetadata *new_file = createFileTags(filename, 2, "1", "2");
	if (new_file == NULL)
		return;

	new_file->next = head;
	head = new_file;
	*fp = new_file;
}

void showFileTags() {
	struct FileMetadata *current = head;
	while (current) {
		fprintf(stdout,
			"| FILE                      | TAG                     "
			"| COUNT     "
			"  |\n"
			"|---------------------------|-------------------------"
			"|--------"
			"-----|\n %s, %s %zu",
			current->filepath,
			current->tags[0] /*XXX loop thru & keep count*/,
			head->file_size /*XXX the tags count */);
	}
	current = current->next;
}

void deleteFileFromListByFilename(const char *path) {
	struct FileMetadata *current = head;
	/*
	XXX delete & relink again
	*/
	while (current) {
		if (strcmp(path, current->filepath) == 0) {
			free(current->filepath);
			for (int i = 0;
			     i < MAX_TAGS && current->tags[i] != NULL; i++) {
				free(current->tags[i]);
			}
			free(current->tags);
			free(current);
		}
		current = current->next;
	}
}

static char filepath[32] = "/tmp/our-tags-XXXXXX";

#include <unistd.h>
char *generateTempJsonFile() {
	int cx = 0;
	if ((cx = mkstemp(filepath)) < 0)
		return 0; /*NULL*/
	// close the fd & make a new filename, with extension & maybe unlink
	close(cx);
	char *buf = malloc(sizeof filepath * 2);
	cx = snprintf(buf, sizeof filepath, "%s%s", filepath, ".json");
	if (cx < 0 && cx >= sizeof filepath)
		goto CLEAN;
	return buf;
CLEAN:
	free(buf);
	return 0;
}

#include <fcntl.h>
/*
Entries of File tags are saved on disk
*/
void persistChangesTempFile(struct FileTagOperations *p) {
	char *path = generateTempJsonFile();

	int fd;
	if (path == 0)
		return;

	/*BUG with NFS*/
	fd = open(path, O_RDWR | O_CREAT | O_APPEND, 0660);
	if (fd < 0)
		return;
/*KEEP FILE size under 4k*/
#define BUFFER sysconf(_SC_PAGE_SIZE)
	if (ftruncate(fd, BUFFER) < 0)
		return;

	char buf[BUFFER];

	// buffer file, with standard c lib
	FILE *stream = fdopen(fd, "rw");
	// XXX fprint persist to the tmp file

	free(path);
}

#ifdef DEBUG
// A copy of w/e is going on w/ temp
#include <bits/getopt_core.h>
#include <sys/mman.h>
void *copyOverSharedMem(char *src,
			/* src to mmap*/ char *dst /* file dst, in our pwd*/) {
	void *addr;
	int fd_src, fd_dst;
	struct stat stats = { 0 };

	if (stat(src, &stats) < 0)
		goto failed;
#define SRC_FILE_SIZE stats.st_size

	fd_src = open(src, O_RDWR | O_CREAT | O_APPEND, 0660);
	addr = mmap(NULL, SRC_FILE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
		    fd_src, 0);

	if (addr == MAP_FAILED)
		goto failed;

	fd_dst = open(dst, O_RDWR | O_CREAT | O_APPEND, 0660);

	if (fd_src < 0 || fd_src < 0)
		goto failed;

	FILE *stream = fdopen(fd_src, "r");

	if (setvbuf(stream, dst, _IOLBF, SRC_FILE_SIZE) != 0)
		goto failed;

	write(fd_dst, addr, SRC_FILE_SIZE);
	fclose(stream);
	/* return for munmap*/
	return addr;
failed:
	return (void *)-1;
}
#endif

int main(int argc, char **argv) {
	int opt;

	struct FileMetadata *fp;

	if (argc < 3)
		usage(argv[0], stderr);

	// TODO , getoptlong & handle all flags
	char *filename;

	char *src = generateTempJsonFile();

	while ((opt = getopt(argc, argv, "a:")) != -1) {
		switch (opt) {
#if defined(DEBUG)
			case 'a': {
				filename = strdup(optarg);
				copyOverSharedMem(src, filename);
				break;
			}
#endif
			default:
				exit(EXIT_FAILURE);
		}
	}
	saveFilesToList(&fp, src);

	showFileTags();
} /* write a malloc track system, smthing like valgrind */
