#include "text.h"
#include "values.h"
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

int read_input(char *data, size_t size)
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

	int value;
	size_t i = 0;
	while ((value = fgetc(stdin)) != EOF && value != '\n' && i < size) {
		data[i] = value;
		i ++;
	}

	if (is_terminal) {
		terminal.c_lflag |= (ECHO | ECHONL);
		if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &terminal) < 0) {
			return -1;
		}
	}

	return 0;
}
