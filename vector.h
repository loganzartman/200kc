#ifndef VECTOR_GUARD
#define VECTOR_GUARD

#define VECTOR_INITIAL_SIZE 8;
typedef struct vector {
	void **cont;
	int size;
	int capacity;
} vector;

#endif

/**
 * Creates and initializes a vector.
 * Returns the vector, or NULL if memory allocation failed.
 */
vector* vector_new();

void vector_free(vector *v);

/**
 * Adds an element to the vector.
 * Returns whether the element was successfully added.
 */
bool vector_add(vector *v, void *item);

/**
 * Gets the element at position index.
 */
void* vector_get(vector *v, int index);

/**
 * Removes the element at position index.
 * Returns the element.
 */
void* vector_remove(vector *v, int index);

/**
 * Resizes the vector if it is at capacity.
 * Returns whether the vector was sucessfully resized.
 */
bool vector_resize(vector *v);