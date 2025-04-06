#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define FILENAMESEM "tempsem"
#define CHECK(x)                     \
	if (x != 0) {                    \
		semctl(sem_id, 0, IPC_RMID); \
		return x;                    \
	}

key_t key_sem;
int sem_id;

struct sembuf up0 = {0, 1, 0};
struct sembuf down0 = {0, -1, 0};

struct sembuf takefork1 = {1, -1, 0};
struct sembuf returnfork1 = {1, 1, 0};

struct sembuf takefork2 = {2, -1, 0};
struct sembuf returnfork2 = {2, 1, 0};

struct sembuf takefork3 = {3, -1, 0};
struct sembuf returnfork3 = {3, 1, 0};

struct sembuf takefork4 = {4, -1, 0};
struct sembuf returnfork4 = {4, 1, 0};

struct sembuf takefork5 = {5, -1, 0};
struct sembuf returnfork5 = {5, 1, 0};

typedef struct philisopher {
	bool take_left_fork;
	bool take_right_fork;
	int id;
} philosopher;

int take_fork(int num, int phil) {
	int status = 0;
	printf("Philosopher %d is trying to take fork %d\n", phil, num % 10);
	switch (num) {
		case 0:
			status = semop(sem_id, &takefork1, 1);
			break;

		case 2:
			status = semop(sem_id, &takefork2, 1);
			break;

		case 4:
			status = semop(sem_id, &takefork3, 1);
			break;

		case 6:
			status = semop(sem_id, &takefork4, 1);
			break;

		case 8:
			status = semop(sem_id, &takefork5, 1);
			break;
		case 10:
			status = semop(sem_id, &takefork1, 1);
			break;
		default:
			printf("error");
			status = 2;
	}
	printf("Fork %d taken by %d\n", num % 10, phil);
	return status;
}

int return_fork(int num) {
	int status = 0;
	switch (num) {
		case 0:
			status = semop(sem_id, &returnfork1, 1);
			break;

		case 2:
			status = semop(sem_id, &returnfork2, 1);
			break;

		case 4:
			status = semop(sem_id, &returnfork3, 1);
			break;

		case 6:
			status = semop(sem_id, &returnfork4, 1);
			break;

		case 8:
			status = semop(sem_id, &returnfork5, 1);
			break;
		case 10:
			status = semop(sem_id, &returnfork1, 1);
			break;
	}
	printf("Fork %d placed back\n", num % 10);
	return status;
}

int main(void) {
	key_sem = ftok(FILENAMESEM, 'K');
	sem_id = semget(key_sem, 6, IPC_CREAT | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
	if (sem_id == -1) {
		return 1;
	}
	int status = 0;
	status = semctl(sem_id, 0, SETVAL, 2);
	CHECK(status);
	status = semctl(sem_id, 1, SETVAL, 1);
	CHECK(status);
	status = semctl(sem_id, 2, SETVAL, 1);
	CHECK(status);
	status = semctl(sem_id, 3, SETVAL, 1);
	CHECK(status);
	status = semctl(sem_id, 4, SETVAL, 1);
	CHECK(status);
	status = semctl(sem_id, 5, SETVAL, 1);
	CHECK(status);
	// return_fork(0);
	// return_fork(2);
	// return_fork(4);
	// return_fork(6);
	// return_fork(8);
	// return_fork(10);
	int i = 1;
	pid_t id;
	pid_t ids[5];
	for (; i < 10; i += 2) {
		id = fork();
		ids[i / 2] = id;
		if (id == 0) {
			break;
		}
		if (id == -1) {
			for (int j = 0; j < i; j += 2) {
				kill(ids[j / 2], SIGKILL);
			}
			semctl(sem_id, 0, IPC_RMID);
			return 1;
		}
	}
	if (id == 0) {
		philosopher p = {false, false, i};
		for (int l = 0; l < 3; l++) {
			status = semop(sem_id, &down0, 1);
			CHECK(status);
			status = take_fork(p.id - 1, p.id);
			CHECK(status);
			// sleep(1);
			status = take_fork(p.id + 1, p.id);
			CHECK(status);
			printf("Philosopher %d is eating\n", p.id);
			sleep(1);
			status = return_fork(p.id - 1);
			CHECK(status);
			status = return_fork(p.id + 1);
			CHECK(status);
			status = semop(sem_id, &up0, 1);
			CHECK(status);
		}
		// for (int l = 0; l < 3; l++)
		// {
		//     take_fork(p.id - 1, p.id);
		//     sleep(1);
		//     take_fork(p.id + 1, p.id);
		//     sleep(1);
		//     return_fork(p.id - 1);
		//     return_fork(p.id + 1);
		// }
	} else {
		int statuschild = 0;
		for (int i = 0; i < 5; i++) {
			waitpid(ids[i], &statuschild, 0);
			if (statuschild != 0) {
				for (int j = i; j < 5; j++) {
					kill(ids[j], SIGKILL);
				}
			}
		}
		status = semctl(sem_id, 0, IPC_RMID);
		CHECK(status);
	}
	return 0;
}