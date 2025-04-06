#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "msg.h"

#define SEND(qid, msg)                                          \
	do {                                                        \
		if (msgsnd(qid, &msg, sizeof(message_text), 0) == -1) { \
			perror("msgsnd");                                   \
			return 1;                                           \
		}                                                       \
	} while (0);

#define RECV(qid, msg)                                         \
	if (msgrcv(qid, &msg, sizeof(message_text), 0, 0) == -1) { \
		perror("msgrcv");                                      \
		return 1;                                              \
	}

static message* parse_cmd_list(const char* const path, int* const cmds_buf_size) {
	if (path == NULL || cmds_buf_size == NULL) {
		return NULL;
	}

	FILE* cmd_list = fopen(path, "r");
	if (cmd_list == NULL) {
		return NULL;
	}

	message* cmds_buf = (message*)malloc(sizeof(message) * 16);
	size_t cmds_buf_cap = 16;

	*cmds_buf_size = 0;

	if (cmds_buf == NULL) {
		fclose(cmd_list);
		return NULL;
	}

	{
		char* line_buf = NULL;
		size_t line_size = 0;

		while (getline(&line_buf, &line_size, cmd_list) > 0) {
			{
				char* newline = strchr(line_buf, '\n');
				if (newline) *newline = '\0';
			}

			char* cmd = strtok(line_buf, " ");
			char* arg = strtok(NULL, " ");

			message msg;
			memset(&msg, 0, sizeof(message));

			bool invalid_cmd = false;

			if (strcmp(cmd, "take") == 0) {
				msg.msg_id = MSG_TAKE;

				if (strcmp(arg, "wolf;") == 0) {
					msg.msg_text.args.obj_id = OBJ_WOLF;
				} else if (strcmp(arg, "goat;") == 0) {
					msg.msg_text.args.obj_id = OBJ_GOAT;
				} else if (strcmp(arg, "cabbage;") == 0) {
					msg.msg_text.args.obj_id = OBJ_CABBAGE;
				} else {
					fprintf(stderr, "invalid object: %s\n", arg);
					invalid_cmd = true;
				}
			} else if (strcmp(cmd, "put;") == 0) {
				msg.msg_id = MSG_PUT;
			} else if (strcmp(cmd, "move;") == 0) {
				msg.msg_id = MSG_MOVE;
			} else {
				fprintf(stderr, "invalid command: %s\n", cmd);
				invalid_cmd = true;
			}

			if (invalid_cmd) {
				free(line_buf);
				free(cmds_buf);
				fclose(cmd_list);
				return NULL;
			}

			if (*cmds_buf_size == cmds_buf_cap) {
				message* new_cmds_buf =
				    (message*)realloc(cmds_buf, cmds_buf_cap * 2 * sizeof(message));

				if (new_cmds_buf == NULL) {
					free(line_buf);
					free(cmds_buf);
					fclose(cmd_list);
					return NULL;
				}

				cmds_buf = new_cmds_buf;
				cmds_buf_cap *= 2;
			}

			cmds_buf[(*cmds_buf_size)++] = msg;
		}

		free(line_buf);
	}

	fclose(cmd_list);
	return cmds_buf;
}

static int main_cleanup(const int exit_code, message* const cmds, const int qid) {
	free(cmds);

	msgctl(qid, IPC_RMID, NULL);

	return exit_code;
}

int main(const int argc, const char* const argv[]) {
	if (argc != 2) {
		fprintf(stderr, "usage: %s <path to command list>\n", argv[0]);
		return 1;
	}

	message* cmds = NULL;
	int cmds_size = 0;
	int qid = -1;

	cmds = parse_cmd_list(argv[1], &cmds_size);
	if (cmds == NULL) {
		fprintf(stderr, "failed to parse command list\n");
		return main_cleanup(1, cmds, qid);
	}

	if ((qid = msgget(IPC_PRIVATE, 0660)) == -1) {
		perror("msgget");
		return main_cleanup(1, cmds, qid);
	}

	key_t server_queue_key;
	if ((server_queue_key = ftok(SERVER_QUEUE_PATH, SERVER_QUEUE_KEY)) == -1) {
		perror("ftok");
		return main_cleanup(1, cmds, qid);
	}

	int server_qid;
	if ((server_qid = msgget(server_queue_key, 0)) == -1) {
		perror("msgget");
		return main_cleanup(1, cmds, qid);
	}

	message msg;
	memset(&msg, 0, sizeof(message));

	msg.msg_id = MSG_INIT;
	msg.msg_text.queue_id = qid;
	SEND(server_qid, msg);

	RECV(qid, msg);
	if (msg.msg_id == MSG_ERR) {
		fprintf(stderr, "registration failed: %s\n", msg.msg_text.args.err_msg);
		return main_cleanup(1, cmds, qid);
	}

	bool all_objects_moved = false;
	bool task_failed = false;

	for (int i = 0; i < cmds_size; i++) {
		cmds[i].msg_text.queue_id = qid;
		SEND(server_qid, cmds[i]);

		RECV(qid, msg);
		if (msg.msg_id == MSG_ERR) {
			fprintf(stderr, "%s\n", msg.msg_text.args.err_msg);
			task_failed = true;
			break;
		} else if (msg.msg_id == MSG_ALL_MOVED) {
			all_objects_moved = true;
			break;
		}
	}

	msg.msg_id = MSG_FIN;
	msg.msg_text.queue_id = qid;
	SEND(server_qid, msg);

	if (all_objects_moved) {
		printf("All objects moved\n");
	} else if (!task_failed) {
		printf(
		    "Some objects are still on the left shore, "
		    "or are in the boat\n");
	}

	return main_cleanup(0, cmds, qid);
}