#pragma once

#define SERVER_QUEUE_PATH "/tmp/sp_task4_mq"
#define SERVER_QUEUE_KEY '4'

#define MSG_INIT 1
#define MSG_TAKE 2
#define MSG_PUT 3
#define MSG_MOVE 4
#define MSG_OK 5
#define MSG_ERR 6
#define MSG_FIN 7
#define MSG_ALL_MOVED 8

typedef enum object_id { OBJ_WOLF, OBJ_GOAT, OBJ_CABBAGE, OBJ_NONE } object_id;

typedef struct message_text {
	int queue_id;
	union {
		object_id obj_id;
		char err_msg[255];
	} args;
} message_text;

typedef struct message {
	long msg_id;
	message_text msg_text;
} message;