#include <ctype.h>
#include <limits.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sem.h>
#include <unistd.h>

#define ll long long

#define EMPTY 0
#define MAN_ONLY 1
#define WOMAN_ONLY 2

struct sembuf up0 = {0, 1, 0};
struct sembuf down0 = {0, -1, 0};
pthread_mutex_t mutex;

int sem_id;
int max_in_room;
_Atomic(int) cur_state = EMPTY;
_Atomic(int) current_number;

bool CheckNumber(char *number, int osn) {
	int start = 0;

	if (number[0] == '-') {
		if (strlen(number) == 1) {
			return false;
		} else {
			start = 1;
		}
	}
	for (int i = start; i < strlen(number); i++) {
		int res = 0;
		if ('0' <= number[i] && number[i] <= '9') {
			res = number[i] - '0';
		} else if (('A' <= number[i] && number[i] <= 'Z') ||
		           ('a' <= number[i] && number[i] <= 'z')) {
			res = 10 + tolower(number[i]) - 'a';
		} else {
			return false;
		}
		if (res >= osn) {
			return false;
		}
	}
	return true;
}

int StringToInt(char *str, int *res) {
	int osn = 10;

	if (!CheckNumber(str, osn)) {
		return 1;
	}
	int sign = 1;
	int start = 0;
	if (str[0] == '-') {
		sign = -1;
		start = 1;
	}
	long long int result = 0;
	for (int i = start; i < strlen(str); i++) {
		if ('0' <= str[i] && str[i] <= '9') {
			if (result > (INT_MAX / osn)) {
				return 1;
			}
			result *= osn;
			result += str[i] - '0';
		} else {
			return 1;
		}
	}
	*res = result * sign;
	return 0;
}

int woman_wants_to_enter(_Atomic(int) *cur_state) {
	while (true) {
		pthread_mutex_lock(&mutex);

		if (*cur_state == EMPTY || *cur_state == WOMAN_ONLY) {
			*cur_state = WOMAN_ONLY;
			if (current_number < max_in_room) {
				current_number++;
				printf("Woman entered\n");
				pthread_mutex_unlock(&mutex);
				break;
			}
		}
		pthread_mutex_unlock(&mutex);
	}
}

int woman_leave(_Atomic(int) *cur_state, int n) {
	pthread_mutex_lock(&mutex);
	current_number--;
	printf("Woman leaved\n");
	if (current_number == 0) {
		*cur_state = EMPTY;
	}
	pthread_mutex_unlock(&mutex);
}

int man_wants_to_enter(_Atomic(int) *cur_state) {
	while (true) {
		// printf("Mai waiting to enter\n");
		pthread_mutex_lock(&mutex);

		if (*cur_state == EMPTY || *cur_state == MAN_ONLY) {
			*cur_state = MAN_ONLY;
			if (current_number < max_in_room) {
				current_number++;
				printf("Man entered\n");
				pthread_mutex_unlock(&mutex);
				break;
			}
		}
		pthread_mutex_unlock(&mutex);
		// printf("Mai waiting to enter unlock\n");
	}
}

int man_leave(_Atomic(int) *cur_state, int n) {
	pthread_mutex_lock(&mutex);
	current_number--;
	printf("Man leaved\n");
	if (current_number == 0) {
		*cur_state = EMPTY;
	}
	pthread_mutex_unlock(&mutex);
}

void *manage_gender(void *gender) {
	char *g = (char *)gender;
	if (strcmp(g, "m") == 0) {
		woman_wants_to_enter(&cur_state);
	} else {
		man_wants_to_enter(&cur_state);
	}
	long a = lrand48();
	a = a % 5;
	// printf("%ld\n", a);
	sleep(a);
	if (strcmp(g, "m") == 0) {
		woman_leave(&cur_state, max_in_room);
	} else {
		man_leave(&cur_state, max_in_room);
	}
}

int main(int argsc, char **args) {
	const int TEST_NUMBER = 10;

	if (argsc != 2) {
		printf("Incorrect number of arguments\n");
		return 2;
	}

	int status = StringToInt(args[1], &max_in_room);
	if (status != 0) {
		printf("Incorrect input\n");
		return 1;
	}

	pthread_t *t = (pthread_t *)malloc((TEST_NUMBER * 2) * sizeof(pthread_t));

	if (t == NULL) {
		return 1;
	}

	pthread_mutex_init(&mutex, NULL);
	srand48(time(NULL));

	for (int i = 0; i < TEST_NUMBER; i++) {
		status = pthread_create(&t[i], NULL, manage_gender, "w");
		if (status != 0) {
			free(t);
			return 1;
		}
		status = pthread_create(&t[TEST_NUMBER + i], NULL, manage_gender, "m");
		if (status != 0) {
			free(t);
			return 1;
		}
	}

	for (int i = 0; i < TEST_NUMBER * 2; i++) {
		status = pthread_join(t[i], NULL);
		if (status != 0) {
			free(t);
			return 1;
		}
	}

	pthread_mutex_destroy(&mutex);
	free(t);
	return 0;
}