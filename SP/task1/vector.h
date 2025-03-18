#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#define TYPE user

typedef struct user{
    char* login;
    int pincode;
	long long sanctions;
	long long sanctions_current;
} user;

struct {
	TYPE* buffer;
	int buffer_size;
	int size;
} typedef vector;
bool vector_create(vector* v, int init_buff_size);
bool vector_is_empty(vector* v);
bool vector_push(vector* v, TYPE d);
void v_set(vector* v, int pos, TYPE val);
TYPE v_get(vector* v, int pos);
bool vector_grow(vector* v);
void vector_free(vector* v);
TYPE* vector_search(vector* v, char* element);
void vector_destroy(vector* v);