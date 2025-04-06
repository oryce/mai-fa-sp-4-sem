#pragma once

#include <stdbool.h>

#define TYPE user

typedef struct user {
	char login[7];
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

bool vector_is_empty(const vector* v);

bool vector_push(vector* v, TYPE d);

void v_set(const vector* v, const int pos, const TYPE val);

TYPE v_get(const vector* v, const int pos);

bool vector_grow(vector* v);

void vector_free(vector* v);

TYPE* vector_search(const vector* v, const char* element);

void vector_destroy(const vector* v);