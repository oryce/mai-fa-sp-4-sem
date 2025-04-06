#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define ll long long int
#define ull unsigned long long int
#define INITIAL_ALLOC 128
#define COPY_EXECUTABLE "./sp_task2_copy"
#define FIND_EXECUTABLE "./sp_task2_find"

void close_files(FILE** files, size_t count) {
	for (int i = 0; i != count; ++i) {
		fclose(files[i]);
	}
}

int xor_blocks(FILE* fd, const size_t block_size, uint8_t* result) {
	const int bytes = ((block_size + 7) / 8);
	// if block_size == 4 then we xor two parts of array buffer
	memset(result, 0, bytes);
	while (1) {
		uint8_t buffer[8];
		const ssize_t read_bytes = fread(buffer, 1, bytes, fd);
		if (read_bytes == -1) {
			return -1;
		}
		if (read_bytes == 0) {
			break;
		}
		for (int i = 0; i < read_bytes; ++i) {
			result[i] ^= buffer[i];
		}
	}
	if (block_size == 4) {
		const uint8_t left_part = result[0] >> 4;
		const uint8_t right_part = result[0] & 0x0F;
		result[0] = left_part ^ right_part;
	}
	return 0;
}

ll count_masked_numbers(FILE* file, const ull mask) {
	ll count = 0;
	ull number = 0;
	while (!feof(file)) {
		fread(&number, 1, sizeof(number), file);
		if (number & mask) {
			if (count < count++) {
				return -1;
			}
		}
	}
	return count;
}

bool copy_file(const size_t count, const char* name_src) {
	const char* dot = strrchr(name_src, '.');
	if (!dot) dot = name_src + strlen(name_src);

	char* base_name = malloc(strlen(name_src));
	if (!base_name) {
		return false;
	}

	const size_t base_len = dot - name_src;
	strncpy(base_name, name_src, base_len);
	base_name[base_len] = '\0';
	const char* extension = dot;

	pid_t* pids = malloc(count * sizeof(pid_t));
	if (!pids) {
		free(base_name);
		return false;
	}
	bool success = true;

	for (size_t i = 0; i < count; ++i) {
		char* new_filename = malloc(strlen(name_src) + 10);
		if (!new_filename) {
			free(base_name);
			free(pids);
			return false;
		}
		snprintf(new_filename, strlen(name_src) + 10, "%s_%zu%s", base_name, i + 1, extension);

		const pid_t pid = fork();
		if (pid == -1) {
			free(new_filename);
			free(base_name);
			free(pids);
			return false;
		}

		if (pid == 0) {
			char* args[] = {COPY_EXECUTABLE, (char*)name_src, new_filename, NULL};
			execvp(args[0], args);
		} else {
			pids[i] = pid;
			free(new_filename);
		}
	}

	for (size_t i = 0; i < count; ++i) {
		int status;
		waitpid(pids[i], &status, 0);

		if (WIFEXITED(status)) {
			if (WEXITSTATUS(status) != 0) {
				success = false;
			}
		} else {
			success = false;
		}
	}

	free(base_name);
	free(pids);
	return success;
}

bool find_string(char** names, FILE** files, const char* str, const int count) {
	pid_t* pids = malloc(count * sizeof(pid_t));
	if (!pids) {
		return false;
	}

	bool success = true;

	for (size_t i = 0; i < count; ++i) {
		const pid_t pid = fork();
		if (pid == -1) {
			free(pids);
			return false;
		}

		if (pid == 0) {
			char* args[] = {FIND_EXECUTABLE, (char*)names[i + 1], (char*)str, NULL};
			execvp(args[0], args);
		} else {
			pids[i] = pid;
		}
	}

	for (size_t i = 0; i < count; ++i) {
		int status;
		waitpid(pids[i], &status, 0);

		if (WIFEXITED(status)) {
			const int exit_code = WEXITSTATUS(status);
			if (exit_code == 0) {
				printf("Found in %s\n", names[i + 1]);
			} else if (exit_code == 1) {
				printf("Not found in %s\n", names[i + 1]);
			} else {
				const char* msg = (exit_code == 2)   ? "Invalid arguments"
				                  : (exit_code == 3) ? "File open error"
				                                     : "Unknown error";
				fprintf(stderr, "Error searching %s: %s\n", names[i + 1], msg);
				success = false;
			}
		} else {
			fprintf(stderr, "Child process failed for %s\n", names[i + 1]);
			success = false;
		}
	}
	free(pids);
	return success;
}

int main(const int argc, char** argv) {
	if (argc < 3) {
		fprintf(stderr, "Usage: ./main {file1} {file2} ... {flag}\n");
		return 1;
	}

	// Make array of file descriptors and open all files
	const int number_of_files = argc - 2;
	FILE** descriptors = malloc(sizeof(FILE*) * number_of_files);
	if (descriptors == NULL) {
		fprintf(stderr, "Out of memory\n");
		return 1;
	}
	for (int i = 1; i <= number_of_files; ++i) {
		descriptors[i - 1] = fopen(argv[i], "r");
		if (descriptors[i - 1] == NULL) {
			fprintf(stderr, "Error opening file: %s\n", argv[i]);
			if (i != 1) {
				close_files(descriptors, i - 2);
			}
			free(descriptors);
			return 1;
		}
	}

	char* flag = argv[argc - 1];
	if (strncmp(flag, "xor", 3) == 0) {
		if (strlen(flag) != 4 || flag[3] - '0' > 6 || flag[3] - '0' < 2) {
			fprintf(stderr, "The xor module should be 2, 3, 4, 5 or 6\n");
			close_files(descriptors, number_of_files);
			free(descriptors);
			return 1;
		}
		const size_t block_size = 1 << (flag[3] - '0');
		for (int i = 0; i != number_of_files; ++i) {
			uint8_t* result = (uint8_t*)malloc((block_size + 7) / 8);
			if (result == NULL) {
				fprintf(stderr, "Out of memory\n");
				close_files(descriptors, number_of_files);
				free(descriptors);
				return 1;
			}
			const int xor_sum = xor_blocks(descriptors[i], block_size, result);
			if (xor_sum == -1) {
				fprintf(stderr, "Error reading from file: %s\n", argv[i + 1]);
			} else {
				printf("Xor sum for file %s : ", argv[i + 1]);
				for (int j = 0; j < (block_size + 7) / 8; ++j) {
					printf("%02x ", result[j]);
				}
				printf("\n");
			}
			free(result);
		}
	} else if (strncmp(flag, "mask", 4) == 0) {
		const char* mask = flag + 4;
		if (strlen(mask) != 8) {
			fprintf(stderr, "The mask must be 4 bytes\n");
			close_files(descriptors, number_of_files);
			free(descriptors);
			return 1;
		}
		char* endptr;
		const ull mask_value = strtoull(mask, &endptr, 16);
		if (*endptr != '\0' || errno == ERANGE) {
			fprintf(stderr, "The mask must be a positive number\n");
			close_files(descriptors, number_of_files);
			free(descriptors);
			return 1;
		}
		for (size_t i = 0; i != number_of_files; ++i) {
			const ll res = count_masked_numbers(descriptors[i], mask_value);
			if (res == -1) {
				printf("Overload long long when counting numbers mask\n");
			} else {
				printf("It was received from %s with mask %lld numbers\n", argv[i + 1], res);
			}
		}
	} else if (strncmp(flag, "copy", 4) == 0) {
		if (strlen(flag) < 5) {
			printf("Usage: ./main {file1} {file2} ... {copyN}\n");
		} else {
			char* endptr;
			const char* num_str = flag + 4;
			long quantity = strtol(num_str, &endptr, 10);
			if (*endptr != '\0' || quantity > 20 || errno == ERANGE) {
				printf("Usage: ./main {file1} {file2} ... {copyN}\n");
				close_files(descriptors, number_of_files);
				free(descriptors);
				return 1;
			}
			for (int i = 0; i != number_of_files; ++i) {
				if (copy_file(quantity, argv[i + 1])) {
					printf("All copies is created for file: %s\n", argv[i + 1]);
				} else {
					fprintf(stderr, "Error copying file: %s\n", argv[i + 1]);
				}
			}
		}
	} else if (strncmp(flag, "find", 4) == 0) {
		char* str = flag + 4;
		if (strlen(str) == 0) {
			fprintf(stderr, "Usage: ./main {file1} {file2} ... {findN}\n");
			close_files(descriptors, number_of_files);
			free(descriptors);
			return 1;
		}
		find_string(argv, descriptors, str, number_of_files);
	} else {
		fprintf(stderr, "Unexpected flag: %s\n", flag);
		close_files(descriptors, number_of_files);
		free(descriptors);
		return 1;
	}
	close_files(descriptors, number_of_files);
	free(descriptors);
	return 0;
}