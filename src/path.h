#include <stdlib.h>

typedef struct
{
	char *path;
	size_t directory_size;
}
Path;

int create_path(Path *, const char *);
char *get_directory(Path *);
char *get_file(Path *);
