#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

bool copy_file(const char* src, const char* dst) {
	FILE* src_file = fopen(src, "rb");
	if (!src_file) return false;

	FILE* dst_file = fopen(dst, "wb");
	if (!dst_file) {
		fclose(src_file);
		return false;
	}

	char buffer[1024];
	size_t bytes;
	while ((bytes = fread(buffer, 1, sizeof(buffer), src_file))) {
		if (fwrite(buffer, 1, bytes, dst_file) != bytes) {
			fclose(src_file);
			fclose(dst_file);
			return false;
		}
	}

	fclose(src_file);
	fclose(dst_file);
	return true;
}

int main(int argc, char** argv) {
	if (argc != 3) {
		return EINVAL;
	}
	if (!copy_file(argv[1], argv[2])) {
		return errno ? errno : EIO;
	}
	return 0;
}
