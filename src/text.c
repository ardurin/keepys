#include "error.h"
#include "text.h"
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

int read_input(String * data)
{
	struct termios terminal;
	int is_terminal = isatty(STDIN_FILENO);
	if (is_terminal) {
		if (tcgetattr(STDIN_FILENO, &terminal) < 0) {
			return -1;
		}
		terminal.c_lflag &= ~(ECHO | ECHONL);
		if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &terminal) < 0) {
			return -1;
		}
	}

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

	if (is_terminal) {
		terminal.c_lflag |= (ECHO | ECHONL);
		if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &terminal) < 0) {
			return -1;
		}
	}

	return 0;
}
