#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void compute_lps(const char *pattern, int *lps, int pattern_len) {
	int len = 0;
	lps[0] = 0;
	for (int i = 1; i < pattern_len; ) {
		if (pattern[i] == pattern[len]) {
			len++;
			lps[i] = len;
			i++;
		} else {
			if (len != 0) {
				len = lps[len - 1];
			} else {
				lps[i] = 0;
				i++;
			}
		}
	}
}

bool kmp_search(FILE *file, const char *pattern) {
	int pattern_len = strlen(pattern);
	if (pattern_len == 0) {
		return false;
	}

	int *lps = (int *)malloc(pattern_len * sizeof(int));
	if (!lps) {
		return false;
	}
	compute_lps(pattern, lps, pattern_len);
	int j = 0;
	int ch;
	while ((ch = fgetc(file)) != EOF) {
		while (j > 0 && ch != pattern[j]) {
			j = lps[j - 1];
		}

		if (ch == pattern[j]) {
			j++;
		}

		if (j == pattern_len) {
			free(lps);
			return true;
		}
	}

	free(lps);
	return false;
}

int main(int argc, char **argv) {
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <filename> <search_string>\n", argv[0]);
		return 2;
	}
	FILE *file = fopen(argv[1], "rb");
	if (!file) {
		perror("Failed to open file");
		return 3;
	}
	const char *search_str = argv[2];
	bool found = kmp_search(file, search_str);
	fclose(file);

	return found ? 0 : 1;
}
