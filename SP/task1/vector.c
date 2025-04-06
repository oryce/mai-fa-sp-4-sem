#pragma once

#include "vector.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

bool vector_create(vector* v, const int init_buff_size) {
	v->size = 0;
	v->buffer_size = init_buff_size;
	v->buffer = malloc(sizeof(TYPE) * init_buff_size);
	if (v->buffer == NULL) {
		return false;
	}
	return true;
}

bool vector_is_empty(const vector* v) { return v->size == 0; }

user* vector_search(const vector* v, const char* element) {
	for (int i = 0; i < v->size; ++i) {
		if (strcmp(v->buffer[i].login, element) == 0) {
			return &v->buffer[i];
		}
	}
	return NULL;
}

bool vector_push(vector* v, const TYPE d) {
	if (v->size == v->buffer_size) {
		if (!vector_grow(v)) {
			return false;
		}
	}
	v->buffer[v->size++] = d;
	return true;
}

void v_set(const vector* v, const int pos, const TYPE val) { v->buffer[pos] = val; }

TYPE v_get(const vector* v, const int pos) { return v->buffer[pos]; }

bool vector_grow(vector* v) {
	TYPE* new_buf = realloc(v->buffer, v->buffer_size * 2 * sizeof(TYPE));
	if (new_buf == NULL) {
		return false;
	}
	v->buffer_size *= 2;
	v->buffer = new_buf;
	return true;
}

void vector_free(vector* v) {
	free(v->buffer);
	v->buffer_size = 0;
	v->size = 0;
}

void vector_destroy(const vector* v) { free(v->buffer); }