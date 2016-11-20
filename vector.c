#include <stdbool.h>
#include <stdlib.h>
#include "vector.h"

vector* vector_new() {
	vector *v = malloc(sizeof(vector));
	v->size = 0;
	v->capacity = VECTOR_INITIAL_SIZE;
	
	//try to allocate memory
	v->cont = calloc(v->capacity, sizeof(void*));
	return v;
}

void vector_free(vector *v) {
	free(v->cont);
	free(v);
}

bool vector_add(vector *v, void *item) {
	vector_resize(v);
	v->cont[v->size] = item;
	v->size++;
}

void* vector_get(vector *v, int index) {
	if (index < 0 || index >= v->size)
		return NULL;
	
	return v->cont[index];
}

void* vector_remove(vector *v, int index) {
	if (index < 0 || index >= v->size)
		return NULL;
	
	void *item = v->cont[index];
	for (int i = index; i < v->size-1; i++) {
		v->cont[i] = v->cont[i+1];
	}
	v->size--;
	return item;
}

bool vector_resize(vector *v) {
	if (v->size >= v->capacity) {
		v->capacity *= 1.5;
		void *newCont = realloc(v->cont, v->capacity * sizeof(void*));
		if (!newCont)
			return false;
		
		v->cont = newCont;
	}
	return true;
}