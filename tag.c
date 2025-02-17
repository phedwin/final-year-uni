
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// TODO this is supposed to be string move to char *[]
enum Tags_Status {
	DRAFT = 0,
	IMPORTANT,
	COMPLETED,
	WORKING_ON,
	PROGRESS,
};

struct FileMetadata {
	int *tags;
	const char *filepath;
	size_t file_size;
	size_t rows;
	struct FileMetadata *next;
};

/*
start of all operations
*/
static struct FileMetadata *head = 0;

struct FileMetadata *create_file_tags(const char *filename,
				      int tags_count,
				      ...) {
	struct FileMetadata *new = malloc(sizeof(struct FileMetadata));
	va_list args;
	va_start(args, tags_count);

	new->filepath = filename;

	struct stat stats;
	memset(&stats, 0, sizeof(struct stat));
	if (stat(filename, &stats))
		return (void *)0;
	new->file_size = 0;
	for (int arg = 0; arg < tags_count; arg++)
		new->tags[arg] = va_arg(args, int);

	return new;
}

struct FileTagOperations {
	char *(*SearchByTag)();
	char *(*searchByFilepath)(struct FileMetadata *);
	void (*ReplaceTags)(enum Tags_Status /*status*/, char * /*file path*/);
	void (*Store)(struct FileMetadata **fp, char *filename);
	void (*CreateFileTags)(const char *filename, int tags_count, ...);
	void (*DeleteFileTags)(char *, enum Tags_Status);
	void (*ReadFileTags)(char *, enum Tags_Status);
};

/*
 *
i decided to make this a linked list because the idea of toml or json is
overkill for this short, so our entries is a bunch of filename linked together
or you know what we can write to a file, with every entries
*/
void saveFilesToList(struct FileMetadata **fp, char *filename) {
	/* we can keep the entries as dynamic as possible but just like i
	 * said its a small project maybe if we get enough time we can
	 * getopt() and -T flag for tags
	 */
	struct FileMetadata *new_file =
	    create_file_tags(filename, 2, DRAFT, COMPLETED);
	new_file->filepath = filename;
	new_file->next = head;
	*fp = new_file;
}

// TODO idea for json
static char *begin = "{";
static char *end = "}";

void saveFilesToText(struct FileMetadata **fp, char *filename) {}

void deleteFilesByName(char *filename) {
	struct FileMetadata *temp = head;
};

// file that contain the tag, just get removed
void deleteFileByTags(enum Tags_Status a);

void searchFileByTags();

int searchByName();
