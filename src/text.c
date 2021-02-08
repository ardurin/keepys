#include "error.h"
#include "text.h"
#include <stdio.h>

int read_input(String * data)
{
	size_t capacity = 40;
	data->size = 0;
	data->text = malloc(capacity);
	if (data->text == NULL)
		return ERROR_MEMORY;
	int character;
	while ((character = fgetc(stdin)) != EOF && character != '\n') {
		if (data->size == capacity) {
			capacity += 20;
			data->text = realloc(data->text, capacity);
			if (data->text == NULL)
				return ERROR_MEMORY;
		}
		data->text[data->size] = character;
		data->size ++;
	}

	return 0;
}
