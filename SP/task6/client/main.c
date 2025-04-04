#include <arpa/inet.h>
#include <endian.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/** MAX_PATH isn't always available and is usually many times
 smaller than this. */
#define MAX_SENDABLE_PATH 16384

static int cleanup(const int exit_code, const int socket_fd) {
	close(socket_fd);
	return exit_code;
}

static int cmd_loop(const int socket_fd) {
	int exit_code = 0;

	char* line_buf = NULL;
	size_t line_buf_size = 0;

	char* res_buf = NULL;
	size_t res_buf_size = 0;

	while (getline(&line_buf, &line_buf_size, stdin) > 0) {
		{
			char* newline = strrchr(line_buf, '\n');
			if (newline) *newline = '\0';
		}

		const size_t line_size = strlen(line_buf) + 1;

		// limit the path length to something sensible
		if (line_size > MAX_SENDABLE_PATH) {
			fprintf(stderr, "Warning: Your path is absurdly long, please correct it.");
			continue;
		}

		// send buffer length
		{
			// XXX: cast size_t into something we can send over the network,
			//      e.g. uint64_t.
			const uint64_t send_size = htobe64(line_size);

			unsigned char buffer[sizeof(send_size)];
			memcpy(buffer, &send_size, sizeof(send_size));

			if (send(socket_fd, buffer, sizeof(buffer), 0) != sizeof(buffer)) {
				perror("send() failed");

				exit_code = 5;
				break;
			}
		}

		// send the buffer
		if (send(socket_fd, line_buf, line_size, 0) != line_size) {
			perror("send() failed");

			exit_code = 5;
			break;
		}

		// read in the response
		uint64_t res_len;

		if (recv(socket_fd, &res_len, sizeof(res_len), MSG_WAITALL) != sizeof(res_len)) {
			perror("recv() failed");

			exit_code = 6;
			break;
		}

		res_len = be64toh(res_len);

		if (res_len > res_buf_size) {
			char* new_res = (char*)realloc(res_buf, res_len);
			if (new_res == NULL) {
				fprintf(stderr, "failed to resize res buffer\n");

				exit_code = 7;
				break;
			}
			res_buf = new_res;
			res_buf_size = res_len;
		}

		if (recv(socket_fd, res_buf, res_len, MSG_WAITALL) != res_len) {
			perror("recv() failed");

			exit_code = 6;
			break;
		}

		printf("%s\n", res_buf);
	}

	free(line_buf);
	free(res_buf);
	return exit_code;
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
		return 2;
	}

	int socket_fd;
	if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket() failed");
		return 3;
	}

	struct sockaddr_in socket_addr;
	socket_addr.sin_family = AF_INET;
	socket_addr.sin_addr.s_addr = host;
	socket_addr.sin_port = htons(port);

	if (connect(socket_fd, (struct sockaddr*)&socket_addr, sizeof(socket_addr)) != 0) {
		perror("connect() failed");
		return 4;
	}

	const int exit_code = cmd_loop(socket_fd);

	return cleanup(exit_code, socket_fd);
}