#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "vector.h"

bool vector_create(vector* v, int init_buff_size) {
	v->size = 0;
	v->buffer_size = init_buff_size;
	v->buffer = malloc(sizeof(TYPE) * init_buff_size);
	if (v->buffer == NULL){
		return false;
	}
	return true;
}

bool vector_is_empty(vector* v) {
	return v->size == 0;
}

user* vector_search(vector* v, char* element){
	for (int i = 0; i < v->size; i++){
		if (strcmp(v->buffer[i].login, element) == 0){
			return &v->buffer[i];
		}
	}
	return NULL;
}

bool vector_push(vector* v, TYPE d) {
	if (v->size == v->buffer){
		bool status = vector_grow(v);
		if (!status){
			return false;
		}
	}	
	v->buffer[v->size] = d;
	v->size++;
	return true;
}

void v_set(vector* v, int pos, TYPE val) {
	v->buffer[pos] = val;
}

TYPE v_get(vector* v, int pos) {
	return v->buffer[pos];
}

bool vector_grow(vector* v) {
	TYPE* new_buf = (TYPE*)realloc(v->buffer, v->buffer_size * 2 * sizeof(TYPE));
	if (new_buf == NULL) {
		return false;
	}
	else {
		v->buffer_size *= 2;
		v->buffer = new_buf;
		return true;
	}
}

void vector_free(vector* v){
	free(v->buffer);
	v->buffer_size = 0;
	v->size = 0;
}

//void vector_shrink(vector* v) {
//	if (v->size == 0) {
//		return;
//	}
//	vector v_temp;
//	vector_create(&v_temp, v->size);
//
//	while (v->size > 0) {
//		vector_push(&v_temp, vector_front(v));
//		vector_pop(v);
//	}
//
//	v->first = 0;
//	while (v_temp.size > 0) {
//		vector_push(v, vector_front(&v_temp));
//		vector_pop(&v_temp);
//	}
//
//	v->buffer = (TYPE*)realloc(v->buffer, v->buffer_size * sizeof(TYPE) * 2 / 3);
//	v->buffer_size = v->buffer_size * 2 / 3;
//}

//TYPE vector_front(vector* v) {
//	return v->buffer[v->first];
//}

void vector_destroy(vector* v) {
	for (int i = 0; i < v->size; i++) {
		free(v->buffer[i].login);
	}
	free(v->buffer);
}