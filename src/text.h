#include <stdlib.h>

typedef struct
{
	char *text;
	size_t size;
}
String;

int read_input(String *);
