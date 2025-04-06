#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "vector.h"

#define ll long long int
#define USERS_FILE "password/passwords.txt"

typedef enum en {
	OK,
	INC_ARGS,
	INC_NUM_ARGS,
	INC_INPUT,
	INC_COMMAND,
	MEM_ALLOC_ERR,
	LOGIN_FAILED,
	LOGIN_SUCCESS,
	LOGOUT,
	NO_SUCH_USER,
	SANCTIONS_APPLIED
} kErrors;

kErrors GoToLogPage(vector* user_vector);

void ProccessError(const kErrors error) {
	switch (error) {
		case INC_ARGS:
			printf("Incorrect arguments\n");
			break;
		case INC_NUM_ARGS:
			printf("INC_NUM_ARGS\n");
			break;
		case INC_INPUT:
			printf("INC_INPUT\n");
			break;
		case INC_COMMAND:
			printf("INC_COMMAND\n");
			break;
		case MEM_ALLOC_ERR:
			printf("MEM_ALLOC_ERR\n");
			break;
		case SANCTIONS_APPLIED:
			printf("You cant use commands due to sanctions, you can only logout\n");
			break;
		default:
			break;
	}
}

void flush() {
	int c = getchar();
	while (c != '\n') {
		c = getchar();
	}
}

kErrors Registrate(vector* user_vector) {
	char name[7];
	printf("Type login\n");
	scanf("%6s", name);

	char check;
	scanf("%c", &check);
	if (check != '\n') {
		return INC_INPUT;
	}
	if (vector_search(user_vector, name) != NULL) {
		printf("User already exists\n");
		return OK;
	}

	printf("Type pincode\n");
	int pincode = 0;
	if (scanf("%d", &pincode) != 1) {
		return INC_INPUT;
	}

	if (pincode < 0 || pincode > 100000) {
		return INC_INPUT;
	}
	scanf("%c", &check);
	if (check != '\n') {
		return INC_INPUT;
	}
	user new_user = {.pincode = pincode, .sanctions = -1};
	strcpy(new_user.login, name);
	if (!vector_push(user_vector, new_user)) {
		return MEM_ALLOC_ERR;
	}
	return OK;
}

kErrors Sanctions(const char* user_string, const char* count, const vector* user_vector) {
	if (strlen(count) > 8) {
		return INC_INPUT;
	}

	char* endptr;
	const ll num = strtol(count, &endptr, 10);

	// is there at least one digit in the number?
	if (endptr == count) {
		return INC_INPUT;
	}
	while (*endptr != '\0') {
		if (!isspace((unsigned char)*endptr)) {
			return INC_INPUT;
		}
		endptr++;
	}
	if (num < 0 || num > INT_MAX) {
		return INC_INPUT;
	}

	user* man = vector_search(user_vector, user_string);
	if (man == NULL) {
		return NO_SUCH_USER;
	}
	printf("Confirm\n");

	char check;
	char command[100] = "";

	scanf("%5s", command);
	scanf("%c", &check);
	if (check != '\n') {
		flush();
		return INC_INPUT;
	}
	if (strcmp(command, "12345") != 0) {
		printf("Confirm failed\n");
	} else {
		printf("Confirm succeed\n");
		man->sanctions = num;
		man->sanctions_current = num + 1;
	}
	return OK;
}

kErrors Login(const vector* user_vector, user** out_user) {
	char name[7];
	printf("Type password\n");
	scanf("%6s", name);

	char check;
	scanf("%c", &check);
	if (check != '\n') {
		return INC_INPUT;
	}

	user* us = vector_search(user_vector, name);
	if (us == NULL) {
		printf("No such user\n");
		return LOGIN_FAILED;
	}

	printf("Type pincode\n");
	int pincode;
	if (scanf("%d", &pincode) != 1) {
		return INC_INPUT;
	}

	if (pincode < 0 || pincode > 100000) {
		return INC_INPUT;
	}

	scanf("%c", &check);
	if (check != '\n') {
		return INC_INPUT;
	}

	if (pincode == us->pincode) {
		printf("Login success\n");
		us->sanctions_current = us->sanctions;
	} else {
		printf("Incorrect pincode\n");
		return LOGIN_FAILED;
	}
	*out_user = us;
	return LOGIN_SUCCESS;
}

struct tm ConvertToTm(const char* time) {
	int d, M, y, h, m, s;
	sscanf(time, "%d:%d:%d %d:%d:%d", &d, &M, &y, &h, &m, &s);
	const struct tm out = {.tm_mday = d,
	                       .tm_mon = M - 1,
	                       .tm_year = y - 1900,
	                       .tm_hour = h,
	                       .tm_min = m,
	                       .tm_sec = s,
	                       .tm_isdst = -1};
	return out;
}

bool ValidateTime(const char* time) {
	int day = 0, month = 0, year = 0, hour = 0, minute = 0, second = 0;
	int nd = 0, nM = 0, ny = 0, nh = 0, nm = 0, ns = 0;

	if (strlen(time) != 19) {
		return false;
	}
	sscanf(time, "%*[0-9]%n:%*[0-9]%n:%*[0-9]%n %*[0-9]%n:%*[0-9]%n:%*[0-9]%n", &nd, &nM, &ny, &nh,
	       &nm, &ns);
	if (nd != 2 || nm != 16 || ny != 10 || nh != 13 || nM != 5 || ns != 19) {
		return false;
	}

	if (sscanf(time, "%d:%d:%d %d:%d:%d", &day, &month, &year, &hour, &minute, &second) != 6) {
		return false;
	}

	if (hour < 0 || hour > 23 || minute < 0 || minute > 59 || second < 0 || second > 59) {
		return false;
	}

	if (year < 1980 || month < 1 || month > 12 || day < 1 || day > 31) {
		return false;
	}
	int max_days;
	switch (month) {
		case 4:
		case 6:
		case 9:
		case 11:
			max_days = 30;
			break;
		case 2:
			if ((year % 400 == 0) || (year % 100 != 0 && year % 4 == 0)) {
				max_days = 29;
			} else {
				max_days = 28;
			}
			break;
		default:
			max_days = 31;
	}

	if (day > max_days) {
		return false;
	}
	return true;
}

struct tm* GetTimeAndDate() {
	const time_t t = time(NULL);
	struct tm* now = localtime(&t);
	return now;
}

void PrintTime() {
	const struct tm* now = GetTimeAndDate();
	char buf[100];
	strftime(buf, 100, "%H:%M:%S", now);
	printf("Time: %s\n", buf);
}

void PrintDate() {
	const struct tm* now = GetTimeAndDate();
	char buf[100];
	strftime(buf, 100, "%d:%m:%Y", now);
	printf("Date: %s\n", buf);
}

long double GetPassedSeconds(const char* ttime, const char* flag) {
	struct tm time_inp = ConvertToTm(ttime);
	struct tm* pointer = &time_inp;
	const time_t now = time(NULL);
	long double res = -1;
	if (strcmp(flag, "-s") == 0) {
		res = difftime(now, mktime(pointer));
	} else if (strcmp(flag, "-m") == 0) {
		res = difftime(now, mktime(pointer)) / 60.0;
	} else if (strcmp(flag, "-h") == 0) {
		res = difftime(now, mktime(pointer)) / 60.0 / 60.0;
	} else if (strcmp(flag, "-y") == 0) {
		res = difftime(now, mktime(pointer)) / 60.0 / 60.0 / 24.0 / 365.0;
	}
	return res;
}

kErrors HowMuch(const char* ttime, const char* flag) {
	if (!ValidateTime(ttime)) {
		return INC_INPUT;
	}
	const long double res = GetPassedSeconds(ttime, flag);
	if (res == -1) {
		return INC_INPUT;
	}
	printf("%Lf\n", res);
	return OK;
}

kErrors ProccessCommands(char* command, vector* v, user* current_user) {
	char* l1 = strtok(command, " ");
	char* l2 = strtok(NULL, " ");
	char* l3 = strtok(NULL, " ");
	char* l4 = strtok(NULL, " ");
	char* l5 = strtok(NULL, " ");

	if (l5 != NULL) {
		return INC_INPUT;
	}

	if (l2 != NULL && l3 == NULL) {
		return INC_NUM_ARGS;
	}

	if (l1 != NULL && l2 == NULL && l3 == NULL && l4 == NULL) {
		if (strcmp(l1, "Logout") == 0) {
			return LOGOUT;
		}

		if (current_user->sanctions_current == 0) {
			return SANCTIONS_APPLIED;
		}

		if (strcmp(l1, "Time") == 0) {
			PrintTime();
		} else if (strcmp(l1, "Date") == 0) {
			PrintDate();
		} else {
			return INC_INPUT;
		}
	} else {
		if (strcmp(l1, "Howmuch") == 0) {
			char* tmp = malloc(256);
			if (tmp == NULL) {
				return MEM_ALLOC_ERR;
			}
			if (l2 != NULL) {
				if (strlen(l2) + strlen(l3) > 256) {
					free(tmp);
					return INC_INPUT;
				}
				strcpy(tmp, "");
				strcat(tmp, l2);
				strcat(tmp, " ");
				strcat(tmp, l3);
			}

			if (l2 != NULL && l4 != NULL) {
				kErrors status = HowMuch(tmp, l4);
				free(tmp);
				return status;
			}
			return INC_INPUT;
		} else if (strcmp(l1, "Sanctions") == 0) {
			if (l2 != NULL && l3 != NULL && l4 == NULL) {
				return Sanctions(l2, l3, v);
			} else {
				return INC_INPUT;
			}
		} else {
			return INC_INPUT;
		}
	}
	return OK;
}

void interaction(user* current_user, vector* user_vector) {
	while (true) {
		char command[128];
		printf("Type command\n");
		scanf("%[^\n]s", command);
		char check;
		scanf("%c", &check);
		if (check != '\n') {
			flush();
		}
		const kErrors status = ProccessCommands(command, user_vector, current_user);
		if (status == LOGOUT) {
			GoToLogPage(user_vector);
			return;
		}
		if (status != OK) {
			ProccessError(status);
		} else {
			current_user->sanctions_current--;
		}
	}
}

kErrors GoToLogPage(vector* user_vector) {
	int c = 0;
	char op;
	char check;
	kErrors status;
	user* cur_user;
	while (true) {
		printf("Type r for registration, l to login, e to exit the application\n");
		c = scanf("%c%c", &op, &check);
		if (c != 2 || check != '\n') {
			ProccessError(INC_INPUT);
			flush();
			continue;
		}
		if (op == 'r') {
			status = Registrate(user_vector);
			if (status != OK && status != INC_INPUT) {
				return status;
			}
			if (status == INC_INPUT) {
				flush();
				ProccessError(status);
			}
		} else if (op == 'l') {
			status = Login(user_vector, &cur_user);
			if (status == LOGIN_FAILED || status == INC_INPUT) {
				ProccessError(status);
				if (status == INC_INPUT) {
					flush();
				}
			} else {
				interaction(cur_user, user_vector);
				return OK;
			}
		} else if (op == 'e') {
			return OK;
		} else {
			ProccessError(INC_COMMAND);
		}
	}
}

int load_users(vector* user_vector) {
	FILE* file = fopen(USERS_FILE, "r");
	if (!file) {
		fclose(file);
		return 1;
	}
	int user_count = 0;
	char login[7];
	int password;
	while (fscanf(file, "%s %d", &login, &password) == 2) {
		user_count++;
		user new_user = {.pincode = password, .sanctions = -1};
		strcpy(new_user.login, login);
		if (!vector_push(user_vector, new_user)) {
			return 1;
		}
	}
	fclose(file);
	return 0;
}

int save_users(const vector* user_vector) {
	FILE* file = fopen(USERS_FILE, "w");
	if (!file) {
		return 1;
	}
	for (int i = 0; i < user_vector->size; i++) {
		user cur = v_get(user_vector, i);
		fprintf(file, "%s %d\n", cur.login, cur.pincode);
	}
	fclose(file);
	return 0;
}

int main(void) {
	vector user_vector;
	vector_create(&user_vector, 20);

	if (load_users(&user_vector)) {
		printf("Error loading users\n");
		return 1;
	}

	GoToLogPage(&user_vector);

	if (save_users(&user_vector)) {
		printf("Error saving users\n");
		return 2;
	}
	vector_destroy(&user_vector);
}