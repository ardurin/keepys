#include "error.h"
#include "path.h"
#include <stdio.h>
#include <string.h>

int create_path(Path *path, const char *name)
{
	char *base = getenv("XDG_DATA_HOME"), *template;
	if (base == NULL) {
		base = getenv("HOME");
		path->directory_size = strlen(base) + 20;
		template = "%s/.local/share/keepys/%s";
	} else {
		path->directory_size = strlen(base) + 7;
		template = "%s/keepys/%s";
	}
	path->path = malloc(path->directory_size + strlen(name) + 2);
	if (path == NULL)
		return ERROR_MEMORY;
	sprintf(path->path, template, base, name);

	return 0;
}

char *get_directory(Path *path)
{
	path->path[path->directory_size] = '\0';
	return path->path;	
}

char *get_file(Path *path)
{
	path->path[path->directory_size] = '/';
	return path->path;	
}
