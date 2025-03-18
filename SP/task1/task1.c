#define _CRT_SECURE_NO_WARNINGS
#include "stdio.h"
#include "stdbool.h"
#include "vector.h"
#include "time.h"
#include "string.h"
#include "limits.h"
#define ll long long int


typedef enum en{
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
kErrors interaction(user* currrent_user, vector* user_vector);

void ProccessError(kErrors error) {

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
    }

}
void flush() {
    char c = getchar();
    while (c != '\n') {
        c = getchar();
    }
}

bool CheckNumber(char* number, int osn) {
    int start = 0;

    if (number[0] == '-') {
        if (strlen(number) == 1) {
            return false;
        }
        else {
            start = 1;
        }
    }
    for (int i = start; i < strlen(number); i++) {
        int res = 0;
        if ('0' <= number[i] && number[i] <= '9') {
            res = number[i] - '0';
        }
        else if (('A' <= number[i] && number[i] <= 'Z') || ('a' <= number[i] && number[i] <= 'z')) {
            res = 10 + tolower(number[i]) - 'a';
        }
        else {
            return false;
        }
        if (res >= osn) {
            return false;
        }
    }
    return true;
}

kErrors StringToInt(char* str, ll* res) {
    int osn = 10;

    if (!CheckNumber(str, osn)) {
        return INC_INPUT;
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
            if (result > (LLONG_MAX / osn)) {
                return INC_INPUT;
            }
            result *= osn;
            result += str[i] - '0';
        }
        else {
            return INC_INPUT;
        }
    }
    *res = result * sign;
    return 0;
}


kErrors Registrate(vector* user_vector){
    int c;
    char* name = (char*)malloc(7);
    if (name == NULL){
        return MEM_ALLOC_ERR;
    }
    printf("Type login\n");
    c = scanf("%6s", name);
    if (c > 6){
        free(name);
        return INC_INPUT;
    }
    char check;
    scanf("%c", &check);
    if (check != '\n'){
        free(name);
        return INC_INPUT;
    }
    if (vector_search(user_vector, name) != NULL){
        printf("User already exists\n");
        free(name);
        return OK;
    }
    
    printf("Type pincode\n");
    int pincode = 0;
    c = scanf("%d", &pincode);
    if (c != 1){
        free(name);
        return INC_INPUT;
    }

    if (pincode < 0 || pincode > 100000){
        free(name);
        return INC_INPUT;
    }
    scanf("%c", &check);
    if (check != '\n') {
        free(name);
        return INC_INPUT;
    }
    user new_user;
    new_user.login = name;
    new_user.pincode = pincode;
    new_user.sanctions = -1;
    if (!vector_push(user_vector, new_user)) {
        free(name);
        return MEM_ALLOC_ERR;
    }
    return OK;
}

kErrors Sanctions(char* user_string, char* count, vector* user_vector) {
    if (strlen(count) > 8) {
        return INC_INPUT;
    }

    ll num;
    kErrors status = StringToInt(count, &num);
    if (status != OK || num < 0) {
        return INC_INPUT;
    }
    user *man = vector_search(user_vector, user_string);
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
    }
    else {
        printf("Confirm succeed\n");
        man->sanctions = num;
        man->sanctions_current = num + 1;
    }
    return OK;
}

kErrors Login(vector* user_vector, user** out_user){
    int c;
    char* name = (char*)malloc(7);
    if (name == NULL){
        return MEM_ALLOC_ERR;
    }
    printf("Type login\n");
    c = scanf("%6s", name);
    if (c > 6){
        free(name);
        return INC_INPUT;
    }
    char check;
    scanf("%c", &check);
    if (check != '\n'){
        free(name);
        return INC_INPUT;
    }
    user* us = vector_search(user_vector, name);
    if (us == NULL){
        printf("No such user\n");
        free(name);
        return LOGIN_FAILED;
    }
    
    printf("Type pincode\n");
    int pincode;
    c = scanf("%d", &pincode);
    if (c != 1){
        free(name);
        return INC_INPUT;
    }

    if (pincode < 0 || pincode > 100000){
        free(name);
        return INC_INPUT;
    }

    scanf("%c", &check);
    if (check != '\n'){
        free(name);
        return INC_INPUT;
    }

    if (pincode == us->pincode){
        printf("Login success\n");
        us->sanctions_current = us->sanctions;
        
    } else {
        printf("Incorrect pincode\n");
        free(name);
        return LOGIN_FAILED;
    }

    free(name);
    *out_user = us;
    return LOGIN_SUCCESS;
}



struct tm ConvertToTm(char* time) {
	int d, M, y, h, m, s;
	struct tm out;
	sscanf(time, "%d:%d:%d %d:%d:%d", &d, &M, &y, &h, &m, &s);
	out.tm_mday = d;	
	out.tm_mon = M - 1;
	out.tm_year = y - 1900;
	out.tm_hour = h;
	out.tm_min = m;
	out.tm_sec = s;
	out.tm_isdst = -1;
	return out;
}

bool ValidateTime(char* time) {
	int d, M, y, h, m, s;
	int nd, nM, ny, nh, nm, ns;

	sscanf(time, "%*[0-9]%n:%*[0-9]%n:%*[0-9]%n %*[0-9]%n:%*[0-9]%n:%*[0-9]%n", &nd, &nM, &ny, &nh, &nm, &ns);

	if (nd != 2 || nm != 16 || ny != 10 || nh != 13 || nM != 5 || ns != 19) {
		return false;
	}

	int count;
	count = sscanf(time, "%d:%d:%d %d:%d:%d", &d, &M, &y, &h, &m, &s);
	if (count != 6) {
		return false;
	}

	if (d < 0 || d > 31 || M < 1 || M > 12 || y < 1980 || h < 0 || h > 23 || m < 0 || m > 59 || s < 0 || s > 59) {
		return false; 
	}
	return true;
}

kErrors PrintTime(){
    time_t t = time(NULL);
    struct tm* now = localtime(&t);
    char buf[100];
    strftime(buf, 100, "%d:%m:%Y:%H:%M:%S\n", now);
    printf("Time: %s", buf);
}

kErrors PrintDate(){
    time_t t = time(NULL);
    struct tm* now = localtime(&t);
    char buf[100];
    strftime(buf, 100, "%d:%m:%Y:%H:%M:%S\n", now);
    printf("Date: %s\n", buf);
}

kErrors HowMuch(char* ttime, char* flag){
    if (!ValidateTime(ttime)){
        return INC_INPUT;
    }
    struct tm time_inp = ConvertToTm(ttime);
    struct tm* pointer = &time_inp;
    time_t now = time(NULL);
    long double res;
    if (strcmp(flag, "-s") == 0){
        res = difftime(now, mktime(pointer));
        printf("%Lf seconds passed\n", res);
    } else if (strcmp(flag, "-m") == 0){
        res = difftime(now, mktime(pointer));
        printf("%Lf minutes passed\n", res / 60.0);
    } else if (strcmp(flag, "-h") == 0){
        res = difftime(now, mktime(pointer));
        printf("%Lf hours passed\n", res/60/60);
    } else if (strcmp(flag, "-y") == 0){
        res = difftime(now, mktime(pointer));
        printf("%Lf years passed\n", res/60/60/24/365);
    } else{
        return INC_INPUT;
    }
    return OK;
}

kErrors ProccessCommands(char* command, vector* v, user* current_user){
    char * l1 = strtok(command, " ");
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
   
    if (l1 != NULL && l2 == NULL && l3 == NULL && l4 == NULL){
        if (strcmp(l1, "Logout") == 0) {
            return LOGOUT;
        }
        
        if (current_user->sanctions_current == 0) {
            return SANCTIONS_APPLIED;
        }

        if (strcmp(l1, "Time") == 0){
            PrintTime();
        } else if (strcmp(l1, "Date") == 0){
            PrintDate();
        }
        else {
            return INC_INPUT;
        }
    }
    else {
        if (strcmp(l1, "Howmuch") == 0) {
            char* tmp = malloc(256);
            if (tmp == NULL) {
                return MEM_ALLOC_ERR;
            }
            if (l2 != NULL && l3 != NULL) {
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
        }
        else if (strcmp(l1, "Sanctions") == 0) {
            if (l2 != NULL && l3 != NULL && l4 == NULL) {
                return Sanctions(l2, l3, v);
            }
            else {
                return INC_INPUT;
            }
            
        }
        else {
            return INC_INPUT;
        }
    }
    return OK;
}

kErrors interaction(user* currrent_user, vector* user_vector){
    int c = 0;
    char* command = (char*)malloc(128);
    if (command == NULL){
        return MEM_ALLOC_ERR;
    }

    char check;
    kErrors status;
    
    while (true)
    {
        printf("Type command\n");
        scanf("%[^\n]s", command);
        scanf("%c", &check);
        if (check != '\n'){
            flush();
        }
        status = ProccessCommands(command, user_vector, currrent_user);
        if (status == LOGOUT) {
            GoToLogPage(user_vector);
            free(command);
            return LOGOUT;
        }
        else if (status != OK){
            ProccessError(status);
        }
        else if (status == OK) {
            currrent_user->sanctions_current--;
        }
    }
    free(command);
    return OK;
}

kErrors GoToLogPage(vector* user_vector){
    int c = 0;
    char op;
    char check;
    kErrors status;
    user* cur_user;
    while (true)
    {
        printf("Type r for registration and l to login\n");
        c = scanf("%c%c", &op, &check);
        if (c != 2 || check != '\n'){
            ProccessError(INC_INPUT);
            flush();
            continue;
        }
        if (op == 'r'){
            status = Registrate(user_vector);
            if (status != OK && status != INC_INPUT){
                return status;
            }
            if (status == INC_INPUT) {
                flush();
                ProccessError(status);
            }
        } else if (op == 'l'){
            status = Login(user_vector, &cur_user);
            if (status == LOGIN_FAILED || status == INC_INPUT){
                ProccessError(status);
                if (status == INC_INPUT) {
                    flush();
                }
                continue;
            } else if (status == LOGIN_SUCCESS){
                status = interaction(cur_user, user_vector);
                if (status == LOGOUT) {
                    return OK;
                }
            } else {
                return status;
            }
        } else if (op == 'e'){
            return OK;
        } else {
            ProccessError(INC_COMMAND);
        }
    }
    return OK;
}

int main(void){
    vector user_vector;
    vector_create(&user_vector, 20);
    GoToLogPage(&user_vector);
    vector_destroy(&user_vector);
    //interaction(&user_vector);
}