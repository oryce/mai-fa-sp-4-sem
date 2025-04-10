#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#include "msg.h"

#define LOG(client_qid, ...)                \
	do {                                    \
		printf("[client %d] ", client_qid); \
		printf(__VA_ARGS__);                \
		printf("\n");                       \
	} while (0);

typedef struct session {
	int queue_id;

	// If an object is in the boat, then their respective bool
	// cannot be checked.
	object_id obj_in_boat;

	bool wolf_on_right;
	bool goat_on_right;
	bool cabbage_on_right;
	bool boat_on_right;

	struct session* next;
} session;

static session* sessions = NULL;

static bool register_client(const int queue_id) {
	session* new_session = (session*)malloc(sizeof(session));

	if (new_session == NULL) {
		return false;
	}

	new_session->queue_id = queue_id;
	new_session->obj_in_boat = OBJ_NONE;
	new_session->wolf_on_right = false;
	new_session->goat_on_right = false;
	new_session->cabbage_on_right = false;
	new_session->boat_on_right = false;

	new_session->next = sessions;
	sessions = new_session;

	return true;
}

static void unregister_client(const int queue_id) {
	session* current = sessions;
	session* prev = NULL;

	while (current != NULL) {
		if (current->queue_id == queue_id) {
			if (prev == NULL) {
				sessions = current->next;
			} else {
				prev->next = current->next;
			}
			free(current);
			return;
		}
		prev = current;
		current = current->next;
	}
}

static void destroy_sessions() {
	session* current = sessions;

	while (current != NULL) {
		session* next = current->next;

		free(current);

		current = next;
	}
}

static session* get_session(const int queue_id) {
	session* current = sessions;

	while (current != NULL) {
		if (current->queue_id == queue_id) {
			return current;
		}
		current = current->next;
	}

	return NULL;
}

static void send_error(const int queue_id, const char* const err_msg) {
	LOG(queue_id, "[!] sending error: %s", err_msg);

	message msg = {.msg_id = MSG_ERR};
	strncpy(msg.msg_text.args.err_msg, err_msg,
	        sizeof(msg.msg_text.args.err_msg)); /* ----------- */
	msgsnd(queue_id, &msg, sizeof(message_text), 0);
}

static void send_ok(const int queue_id) {
	LOG(queue_id, "[+] sending confirmation");

	message msg = {.msg_id = MSG_OK};
	msgsnd(queue_id, &msg, sizeof(message_text), 0);
}

static void send_all_moved(const int queue_id) {
	LOG(queue_id, "[+] sending all moved");

	message msg = {.msg_id = MSG_ALL_MOVED};
	msgsnd(queue_id, &msg, sizeof(message_text), 0);
}

static void handle_init(const message msg) {
	const int queue_id = msg.msg_text.queue_id;
	LOG(queue_id, "[+] creating session");

	if (!register_client(queue_id)) {
		send_error(queue_id, "Failed to register client");
		return;
	}

	send_ok(queue_id);
}

static void handle_take(const message msg) {
	const int queue_id = msg.msg_text.queue_id;
	LOG(queue_id, "[>] request: take");

	session* session = get_session(queue_id);
	if (session == NULL) {
		send_error(queue_id, "Not registered");
		return;
	}

	if (session->obj_in_boat != OBJ_NONE) {
		send_error(queue_id, "Object already in boat");
		return;
	}

	session->obj_in_boat = msg.msg_text.args.obj_id;

	send_ok(queue_id);
}

static void handle_put(const message msg) {
	const int queue_id = msg.msg_text.queue_id;
	LOG(queue_id, "[>] request: put");

	session* session = get_session(queue_id);
	if (session == NULL) {
		send_error(queue_id, "Not registered");
		return;
	}

	if (session->obj_in_boat == OBJ_NONE) {
		send_error(queue_id, "No object in boat");
		return;
	}

	session->obj_in_boat = OBJ_NONE;

	// Check if all objects were moved to the right shore.
	bool all_objects_moved = session->wolf_on_right && session->goat_on_right &&
	                         session->cabbage_on_right && session->boat_on_right;
	if (all_objects_moved) {
		send_all_moved(queue_id);
		return;
	}

	send_ok(queue_id);
}

static void handle_move(const message msg) {
	const int queue_id = msg.msg_text.queue_id;
	LOG(queue_id, "[>] request: move");

	session* session = get_session(queue_id);
	if (session == NULL) {
		send_error(queue_id, "Not registered");
		return;
	}

	session->boat_on_right = !session->boat_on_right;

	// Update object's position on shore.
	switch (session->obj_in_boat) {
		case OBJ_WOLF:
			session->wolf_on_right = session->boat_on_right;
			break;
		case OBJ_GOAT:
			session->goat_on_right = session->boat_on_right;
			break;
		case OBJ_CABBAGE:
			session->cabbage_on_right = session->boat_on_right;
			break;
		case OBJ_NONE:
			break;
	}

	// Check if the wolf and goat are left unsupervised.
	if (session->wolf_on_right == session->goat_on_right &&
	    session->wolf_on_right != session->boat_on_right) {
		send_error(queue_id, "Goat eaten");
		return;
	}

	// Check if the goat and cabbage are left unsupervised.
	if (session->goat_on_right == session->cabbage_on_right &&
	    session->goat_on_right != session->boat_on_right) {
		send_error(queue_id, "Cabbage eaten");
		return;
	}

	send_ok(queue_id);
}

static void handle_fin(const message msg) {
	const int queue_id = msg.msg_text.queue_id;
	LOG(queue_id, "[-] closing session");

	unregister_client(queue_id);
}

int main(const int argc, const char* const argv[]) {
	key_t msg_queue_key;
	if ((msg_queue_key = ftok(SERVER_QUEUE_PATH, SERVER_QUEUE_KEY)) == -1) {
		perror("ftok");
		return 1;
	}

	int msg_qid;
	if ((msg_qid = msgget(msg_queue_key, 0660 | IPC_CREAT)) == -1) {
		perror("msgget");
		return 1;
	}

	printf("Listening for messages on queue %d\n", msg_qid);

	while (1) {
		message msg;
		if (msgrcv(msg_qid, &msg, sizeof(message_text), 0, 0) == -1) {
			perror("msgrcv");
			return 1;
		}

		switch (msg.msg_id) {
			case MSG_INIT:
				handle_init(msg);
				break;
			case MSG_TAKE:
				handle_take(msg);
				break;
			case MSG_PUT:
				handle_put(msg);
				break;
			case MSG_MOVE:
				handle_move(msg);
				break;
			case MSG_FIN:
				handle_fin(msg);
				break;
			default:
				printf("unknown message id: %ld\n", msg.msg_id);
				break;
		}
	}

	destroy_sessions();
	msgctl(msg_qid, IPC_RMID, NULL);
}