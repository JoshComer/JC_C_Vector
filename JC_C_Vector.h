#ifndef JC_C_VECTOR_H_FILE
#define JC_C_VECTOR_H_FILE
#include <stdlib.h>
#include <stdbool.h>
//#include <stdarg.h> not needed now, will be in the future

#define JC_C_VECTOR_RESIZE_FACTOR 2
#define JC_C_VECTOR_GROW_VECTOR(vector) JC_vector_reserve(vector, vector->capacity * JC_C_VECTOR_RESIZE_FACTOR)
#define JC_C_VECTOR_GROW_FAILURE false

#define JC_C_VECTOR_MIN_ELEMENTS 20
#define JC_C_VECTOR_MAX_SIZE 1000000 // Arbitrarily set. No particular reason for this size, so feel free to change it




typedef struct JC_Vector
{
	size_t capacity;
	size_t allocated;
	size_t type_size;

	char* data;


}
JC_Vector;






// ---------------------------------------------------------------------------
//							Setup and Cleanup
// ---------------------------------------------------------------------------

inline JC_Vector* JC_vector_construct(size_t size, size_t type_size)
{
	JC_Vector* new_vector = malloc(sizeof(JC_Vector));

	if (new_vector == NULL)
	{
		return NULL;
	}

	if (size < JC_C_VECTOR_MIN_ELEMENTS)
	{
		size = JC_C_VECTOR_MIN_ELEMENTS;
	}

	if (size * type_size > JC_C_VECTOR_MAX_SIZE)
	{
		return NULL;
	}

	new_vector->capacity = size;
	new_vector->type_size = type_size;
	new_vector->allocated = 0;

	if (size == 0) {
		new_vector->data = NULL;
	}
	else {
		new_vector->data = malloc(size * type_size);
	}

	return new_vector;
}

inline void JC_vector_destruct(JC_Vector** const restrict vector)
{
	if (vector == NULL || *vector == NULL)
		return;

	free((*vector)->data);

	free(*vector);
	*vector = NULL;
}






// --------------------------------------------------------------------------------
//									Element Access
// --------------------------------------------------------------------------------

inline char* JC_vector_at_ptr(const JC_Vector* const restrict vector, const size_t index) {
	if (index >= vector->allocated) {
		return NULL;
	}

	return vector->data + (index * vector->type_size);
}


inline char* JC_vector_at_ptr_unsafe(const JC_Vector* const restrict vector, const size_t index) {
	return vector->data + (index * vector->type_size);
}


inline char* JC_vector_front(const JC_Vector* const restrict vector)
{
	if (vector->allocated == 0)
		return NULL;

	return vector->data;
}


inline char* JC_vector_back(const JC_Vector* const restrict vector)
{
	if (vector->allocated == 0)
		return NULL;

	return vector->data + ((vector->allocated - 1) * vector->type_size);
}

inline char* JC_vector_data(const JC_Vector* const restrict vector)
{
	return vector->data;
}






// -----------------------------------------------------------------------------
//								Iterators
// -----------------------------------------------------------------------------

inline char* JC_vector_begin(const JC_Vector* const restrict vector)
{
	return vector->data;
}

inline const char* JC_vector_cbegin(const JC_Vector* const restrict vector)
{
	return vector->data;
}

inline char* JC_vector_end(const JC_Vector* const restrict vector)
{
	return vector->data + (vector->allocated * vector->type_size);
}

inline const char* JC_vector_cend(const JC_Vector* const restrict vector)
{
	return vector->data + (vector->allocated * vector->type_size);
}






// -----------------------------------------------------------------------------
//									Capacity
// -----------------------------------------------------------------------------

inline bool JC_vector_empty(const JC_Vector* const restrict vector)
{
	return !vector->allocated;
}

inline size_t JC_vector_size(const JC_Vector* const restrict vector)
{
	return vector->allocated;
}

inline size_t JC_vector_max_size()
{
	return JC_C_VECTOR_MAX_SIZE;
}


inline bool JC_vector_reserve(JC_Vector* const restrict vector, const size_t size)
{

	if (size <= vector->capacity)
		return true;

	if (size * vector->type_size > JC_C_VECTOR_MAX_SIZE)
		return false;


	void* temp_data = malloc(size * vector->type_size);

	if (temp_data == NULL) {
		return false;
	}

	memcpy(temp_data, vector->data, vector->capacity * vector->type_size);

	free(vector->data);
	vector->data = temp_data;
	vector->capacity = size;

	return true;

}


inline size_t JC_vector_capacity(const JC_Vector* const restrict vector)
{
	return vector->capacity;
}


inline bool JC_vector_shrink_to_fit(JC_Vector* restrict vector)
{
	void* new_data;

	if (vector->allocated <= 1)
	{
		new_data = realloc(vector->data, vector->type_size);
	}
	else
	{
		new_data = realloc(vector->data, vector->allocated * vector->type_size);
	}


	if (new_data == NULL)
		return false;

	vector->data = new_data;
	vector->capacity = vector->allocated;
	return true;
}






// -----------------------------------------------------------------------------
//									Modifiers
// -----------------------------------------------------------------------------


inline void JC_vector_clear(JC_Vector* const restrict vector)
{
	// TODO - When updating to add object oriented features, update here to use destructor
	vector->allocated = 0;
}


inline char* JC_vector_insert_ptr(JC_Vector* const restrict vector, const size_t index, const void* const restrict value)
{
	if (index > vector->allocated)
		return NULL;

	if (vector->allocated == vector->capacity)
	{
		if (JC_C_VECTOR_GROW_VECTOR(vector) == JC_C_VECTOR_GROW_FAILURE)
		{
			return NULL;
		}
	}

	char* insert_position = vector->data + (index * vector->type_size);

	// move all other data to make room for the inserted element
	memmove(insert_position + (1 * vector->type_size), insert_position, (vector->allocated - index) * vector->type_size);

	// insert the value into position
	memcpy(insert_position, value, vector->type_size);

	vector->allocated++;

	return insert_position;
}


inline char* JC_vector_erase(JC_Vector* const restrict vector, const size_t index)
{
	if (index >= vector->allocated)
		return NULL;

	char* erase_position = vector->data + (index * vector->type_size);

	memmove(erase_position, erase_position + vector->type_size, (vector->allocated - index - 1) * vector->type_size);

	vector->allocated--;
	return erase_position;
}


inline bool JC_vector_pushback_ptr(JC_Vector* const restrict vector, const void* const restrict data)
{

	if (vector->allocated == vector->capacity)
	{
		if (JC_C_VECTOR_GROW_VECTOR(vector) == JC_C_VECTOR_GROW_FAILURE)
		{
			return false;
		}
	}

	memcpy(vector->data + (vector->allocated * vector->type_size), data, vector->type_size);
	vector->allocated++;

	return true;
}


inline void JC_vector_pop_back(JC_Vector* const restrict vector)
{
	if (vector->allocated == 0)
		return;

	vector->allocated--;
}


inline bool JC_vector_resize(JC_Vector* const restrict vector, const size_t new_size)
{

	if (new_size <= vector->allocated)
	{
		vector->allocated = new_size;
		return true;
	}

	while (new_size > vector->capacity)
	{
		if (JC_C_VECTOR_GROW_VECTOR(vector) == JC_C_VECTOR_GROW_FAILURE)
			return false;
	}

	size_t allocated_difference = new_size - vector->allocated;
	// "default-inserted" value is simply assumed to be zero in the case no explicit value is provided
	memset(vector->data + vector->allocated, 0, allocated_difference * vector->type_size);

	vector->allocated = new_size;
	return true;
}


inline bool JC_vector_resize_ptr(JC_Vector* const restrict vector, const size_t new_size, const void* const restrict default_value)
{

	if (new_size <= vector->allocated)
	{
		vector->allocated = new_size;
		return true;
	}

	while (new_size > vector->capacity)
	{
		if (JC_C_VECTOR_GROW_VECTOR(vector) == JC_C_VECTOR_GROW_FAILURE)
			return false;
	}

	size_t allocated_difference = new_size - vector->allocated;

	for (size_t i = vector->allocated; i < new_size; i++)
	{
		memcpy(vector->data + (i * vector->type_size), default_value, vector->type_size);
	}

	vector->allocated = new_size;
	return true;
}


void JC_vector_swap(JC_Vector** const restrict vector1, JC_Vector** const restrict vector2)
{
	void* temp_ptr = *vector1;
	*vector1 = *vector2;
	*vector2 = temp_ptr;
}






// --------------------------------------------------------------------------------
//						Other functions (non member in C++)
// --------------------------------------------------------------------------------

bool JC_vector_are_same_shallow(JC_Vector* const restrict vector1, JC_Vector* const restrict vector2)
{

	if (vector1->allocated != vector2->allocated)
		return false;
	else if (vector1->type_size != vector2->type_size)
		return false;


	if (memcmp(vector1->data, vector2->data, vector1->allocated * vector1->type_size) == 0)
		return true;


	return false;
}


inline int JC_vector_erase_if_same(JC_Vector* const restrict vector, const void* const restrict value)
{
	int num_erased = 0;

	for (size_t i = 0; i < vector->allocated; i++)
	{
		if (memcmp(value, JC_vector_at_ptr(vector, i), vector->type_size) == 0)
		{
			JC_vector_erase(vector, i);
			num_erased++;
			i--;
		}
	}

	return num_erased;
}


inline int JC_vector_erase_if_predicate(JC_Vector* const restrict vector, bool predicate_function())
{
	int num_erased = 0;

	for (size_t i = 0; i < vector->allocated; i++)
	{
		if (predicate_function(vector->data + (i * vector->type_size)))
		{
			JC_vector_erase(vector, i);
			num_erased++;
			i--;
		}
	}
}






// --------------------------------------------------------------------------------
//		debug functions -- Change the pointer type and print statement to use
// --------------------------------------------------------------------------------

void JC_vector_safe_dump(const JC_Vector* const restrict vector)
{
	for (size_t i = 0; i < vector->allocated; i++)
	{
		int* temp_ptr = (int*)JC_vector_at_ptr(vector, i);
		printf("%d:%d\n", i, *temp_ptr);
	}
}


void JC_vector_unsafe_dump(const JC_Vector* const restrict vector)
{
	for (size_t i = 0; i < vector->capacity; i++)
	{
		int* temp_ptr = (int*)JC_vector_at_ptr_unsafe(vector, i);
		printf("%d: num:%d\n", i, *temp_ptr);
	}
}


#endif