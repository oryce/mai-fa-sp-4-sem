#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define INITIAL_ALLOC 128

char* read_line(FILE* fin) {
	size_t bufsize = INITIAL_ALLOC;
	size_t read_chars = 0;
	char* line = malloc(bufsize);
	if (!line) return NULL;

	int ch;
	while ((ch = fgetc(fin)) != EOF) {
		if (read_chars + 1 >= bufsize) {
			bufsize *= 2;
			char* tmp = realloc(line, bufsize);
			if (!tmp) {
				free(line);
				return NULL;
			}
			line = tmp;
		}
		if (ch == '\n') break;
		line[read_chars++] = ch;
	}

	if (read_chars == 0 && ch == EOF) {
		free(line);
		return NULL;
	}

	line[read_chars] = '\0';
	return line;
}

int main(int argc, char** argv) {
	if (argc != 3) {
		return 2; // Special code for invalid arguments
	}

	FILE* file = fopen(argv[1], "r");
	if (!file) {
		return 3; // Special code for file open error
	}

	const char* search_str = argv[2];
	int line_number = 0;
	int found_line = -1;
	char* line;

	while ((line = read_line(file))) {
		line_number++;
		if (strstr(line, search_str)) {
			found_line = line_number;
			free(line);
			break;
		}
		free(line);
	}

	fclose(file);
	return (found_line == -1) ? 1 : 0;
}
