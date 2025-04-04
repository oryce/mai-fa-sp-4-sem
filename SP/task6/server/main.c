#include <arpa/inet.h>
#include <dirent.h>
#include <endian.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define CONNECTIONS_BACKLOG 5
#define MAX_PATH 16384  // see client/main.c on path limits

#define LOG(client_fd, ...)                \
	do {                                   \
		printf("[client %d] ", client_fd); \
		printf(__VA_ARGS__);               \
		printf("\n");                      \
	} while (0);

static int cleanup(const int exit_code, const int server_fd) {
	close(server_fd);
	return exit_code;
}

static void send_string(const int client_fd, const char* const msg) {
	LOG(client_fd, "send response: %s", msg);

	const uint64_t msg_length = strlen(msg) + 1;
	const uint64_t sent_length = htobe64(msg_length);

	{
		char buffer[sizeof(sent_length)];
		memcpy(buffer, &sent_length, sizeof(sent_length));

		if (write(client_fd, buffer, sizeof(buffer)) != sizeof(buffer)) {
			LOG(client_fd, "cannot write response");
			return;
		}
	}

	if (write(client_fd, msg, msg_length) != msg_length) {
		LOG(client_fd, "cannot write response");
	}
}

static char* list_files(const char* const root_path) {
	DIR* dir = opendir(root_path);
	if (dir == NULL) return NULL;

	char* res_buf = NULL;
	size_t res_size = 0;

	struct dirent* ent;
	while ((ent = readdir(dir))) {
		char abs_ent_path[MAX_PATH] = {'\0'};

		if (*root_path == '/' && *(root_path + 1) == '\0') {
			// Appending "root_path" adds another "/" here, which is unwanted.
			snprintf(abs_ent_path, sizeof(abs_ent_path), "/%s", ent->d_name);
		} else {
			snprintf(abs_ent_path, sizeof(abs_ent_path), "%s/%s", root_path, ent->d_name);
		}

		const size_t abs_ent_size = strlen(abs_ent_path) + 1;
		char* resized_buf = (char*)realloc(res_buf, res_size + abs_ent_size + 1);

		if (resized_buf == NULL) {
			closedir(dir);
			free(res_buf);
			return NULL;
		}

		if (res_buf == NULL) {
			resized_buf[0] = '\0';
		}

		res_buf = resized_buf;
		res_size += abs_ent_size + 1;

		strncat(res_buf, abs_ent_path, abs_ent_size - 1);
		strcat(res_buf, "\n");
	}

	closedir(dir);
	return res_buf;
}

static void process_client(const int client_fd) {
	char* msg_buf = NULL;
	size_t msg_buf_size = 0;
	uint64_t msg_len;

	while (recv(client_fd, &msg_len, sizeof(msg_len), MSG_WAITALL) == sizeof(msg_len)) {
		if (msg_len == 0) {
			continue;
		}

		msg_len = be64toh(msg_len);
		LOG(client_fd, "read message of %zu bytes", msg_len);

		// Resize the buffer to fit the new message.
		if (msg_len > msg_buf_size) {
			char* new_msg_buf = (char*)realloc(msg_buf, msg_len);
			if (new_msg_buf == NULL) {
				fprintf(stderr, "failed to resize msg buffer\n");
				break;
			}
			msg_buf = new_msg_buf;
			msg_buf_size = msg_len;
		}

		// Read the full message.
		if (recv(client_fd, msg_buf, msg_len, MSG_WAITALL) != msg_len) {
			LOG(client_fd, "failed to read message");
			break;
		}

		LOG(client_fd, "read message: %s", msg_buf);

		// Retrieve absolute path.
		char resolved_path[MAX_PATH];
		if (realpath(msg_buf, resolved_path) == NULL) {
			send_string(client_fd, "no such path exists");
			continue;
		}

		// Validate the path.
		if (strcmp(msg_buf, resolved_path) != 0) {
			send_string(client_fd, "not an absolute path");
			continue;
		}
		{
			struct stat path_stat;
			stat(resolved_path, &path_stat);
			if (!S_ISDIR(path_stat.st_mode)) {
				send_string(client_fd, "not a directory");
				continue;
			}
		}

		char* listing = list_files(resolved_path);

		if (listing == NULL) {
			send_string(client_fd, "failed to list files");
		} else {
			send_string(client_fd, listing);
			free(listing);
		}
	}

	free(msg_buf);
}

int main(const int argc, const char* const argv[]) {
	if (argc != 3) {
		printf("usage: %s <host> <port>\n", argv[0]);
		return 0;
	}

	const char* host_in = argv[1];
	const char* port_in = argv[2];

	long port;
	{
		char* endp;

		errno = 0;
		port = strtol(port_in, &endp, 10);

		if (*endp != '\0' || errno == ERANGE || port < 0 || port > 65535) {
			fprintf(stderr, "Invalid port number: %s\n", port_in);
			return 1;
		}
	}

	in_addr_t host;
	if (inet_pton(AF_INET, host_in, &host) != 1) {
		perror("can't parse host address");
		return 6;
	}

	int server_fd;
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket() failed");
		return 2;
	}

	{
		int opt = 1;
		if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) != 0) {
			perror("setsockopt() failed");
			return cleanup(7, server_fd);
		}
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = host;
	addr.sin_port = htons(port);

	socklen_t addr_len = sizeof(addr);

	if (bind(server_fd, (struct sockaddr*)&addr, addr_len) < 0) {
		perror("bind() failed");
		return cleanup(3, server_fd);
	}

	if (listen(server_fd, CONNECTIONS_BACKLOG) < 0) {
		perror("listen() failed");
		return cleanup(4, server_fd);
	}

	printf("Listening on port %ld\n", port);

	while (1) {
		int client_fd;
		if ((client_fd = accept(server_fd, (struct sockaddr*)&addr, &addr_len)) < 0) {
			if (errno == EINTR) {
				// accept() interrupted
				return cleanup(0, server_fd);
			}
			perror("accept() failed");
			return cleanup(5, server_fd);
		}

		LOG(client_fd, "accepted connection");
		process_client(client_fd);

		LOG(client_fd, "connection ends");
		close(client_fd);
	}
}