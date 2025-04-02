#include "secret.h"
#include "text.h"
#include "values.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
	const char *name;
	int (*function)(Path *, const char *);
}
Command;

static inline void start(char **argv, int *status)
{
	Command available[] = {
		{ "get", get_secret },
		{ "create", create_secret },
	};
	for (Command *command = available; command < available + 2; command ++) {
		if (strcmp(argv[1], command->name) == 0) {
			char password[SIZE_PASSWORD] = {0};
			if ((*status = read_input(password, SIZE_PASSWORD)) < 0)
				return;
			Path path = {0};
			if ((*status = create_path(&path, argv[2])) < 0)
				return;
			*status = command->function(&path, password);
			free((void *)path.path);
			return;
		}
	}
}

int main(int argc, char **argv)
{
	int status = ERROR_ARGUMENTS;
	if (argc == 3)
		start(argv, &status);
	switch (status) {
		case ERROR_ARGUMENTS:
			fprintf(stderr, "Error: invalid arguments\n");
			break;
		case ERROR_FILE:
			fprintf(stderr, "Error: cannot open %s\n", argv[2]);
			break;
		case ERROR_GENERATOR:
			fputs("Error: failed to generate random number\n", stderr);
			break;
		case ERROR_MEMORY:
			fputs("Error: failed to allocate memory\n", stderr);
			break;
		case ERROR_PASSWORD:
			fputs("Error: incorrect password\n", stderr);
			break;
	}
	return status == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
