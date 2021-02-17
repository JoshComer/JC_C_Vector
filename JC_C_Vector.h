#ifndef _JC_C_VECTOR_H
#define _JC_C_VECTOR_H
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define JC_C_VECTOR_RESIZE_FACTOR 2
#define JC_C_VECTOR_GROW_VECTOR(vector) JC_vector_reserve(vector, vector->capacity * JC_C_VECTOR_RESIZE_FACTOR)
#define JC_C_VECTOR_ERROR -7
#define JC_C_VECTOR_SUCCESS 1

typedef struct Vector 
{
	size_t capacity;
	size_t type_size;
	size_t allocated;
	void* data;
}
Vector;


// Setup and Cleanup
// ---------------------------------------------------------------------------

inline Vector JC_vector_construct(const size_t size, const size_t type_size) {
	Vector new_vector;

	new_vector.capacity = size;
	new_vector.type_size = type_size;
	new_vector.allocated = 0;

	if (size == 0) {
		new_vector.data = NULL;
	}
	else {
		new_vector.data = malloc(size * type_size);
	}
	
	return new_vector;
}

inline void JC_vector_destruct(Vector* const restrict vector) {
	free(vector->data);
}


// Element Access
// --------------------------------------------------------------------------------

inline void* JC_vector_at_ptr(const Vector* const restrict vector, const size_t index) {
	if (index >= vector->allocated) {
		return NULL;
	}

	return (char*)vector->data + (index * vector->type_size);
}

inline void* JC_vector_at_ptr_unsafe(const Vector* const restrict vector, const size_t index) {
	return (char*)vector->data + (index * vector->type_size);
}


// front

// back


// Iterators
// -----------------------------------------------------------------------------

// begin
// end
// rbegin
// rend



// Capacity
// -----------------------------------------------------------------------------

inline bool JC_vector_empty(Vector* const restrict vector) {

	return !vector->allocated;

}

inline size_t JC_vector_size(Vector* const restrict vector) {

	return vector->allocated;

}


// max_size

inline bool JC_vector_reserve(Vector* const restrict vector, const size_t size) {

	if (size <= vector->capacity) {
		return true;
	}

	void* temp_data = malloc(size * vector->type_size);

	if (temp_data == NULL) {
		return false;
	}

	memcpy(temp_data, vector->data, vector->capacity);

	free(vector->data);
	vector->data = temp_data;

	return true;

}

inline size_t JC_vector_capacity(const Vector* const restrict vector) {
	return vector->capacity;
}

// shrink_to_fit

// Modifiers
// -----------------------------------------------------------------------------


// clear

// insert

// emplace

// erase

inline bool JC_vector_pushback_ptr(Vector* const restrict vector, const void* const restrict data) {

	if (vector->allocated == vector->capacity) {
		if (JC_C_VECTOR_GROW_VECTOR(vector) == false) {
			return false;
		}
	}

	memcpy((char*)vector->data + (vector->allocated * vector->type_size), data, vector->type_size);
	vector->allocated++;

	return true;
}

// emplace_back

// pop_back

// resize

// swap


// Other functions (non member in C++)
// --------------------------------------------------------------------------------

// comparators?

// erase_if










// TO ADD NEW TYPES - ADD TO BOTH JC_C_VECTOR_GENERIC_ALL_TYPES AND JC_C_VECTOR_FUNCTION_DEFINITION_ALL_TYPES
// macros to assist other macros in function generation
// ----------------------------------------------------
#define JC_C_VECTOR_GENERIC_ALL_TYPES(function) int: function##_int, double: function##_double, long: function##_long, char: function##_char

// macro(type, type_string) type_string is to append to the end of function names
#define JC_C_VECTOR_FUNCTION_DEFINITION_ALL_TYPES(macro) macro(int, int); macro(unsigned int, unsigned_int); macro(double, double); macro(long double, long_double); macro(long, long); macro(long long, long_long); macro(char, char); macro(unsigned char, unsigned_char); macro(short, short); macro(unsigned short, unsigned_short); macro(size_t, size_t); 






// All JC_vector_pushback macros. Call macro, definition macro, and all types generation macro

#define JC_vector_pushback(vector, type) _Generic((type), JC_C_VECTOR_GENERIC_ALL_TYPES(JC_vector_pushback))(vector, type)

#define JC_VECTOR_PUSHBACK_FUNCTION(data_type, data_type_string) bool JC_vector_pushback_##data_type_string(Vector * const vector, data_type data) {\
	if (vector->allocated == vector->capacity) { \
		if (JC_C_VECTOR_GROW_VECTOR(vector) == false) { \
			return false; \
		} \
	} \
 \
	memcpy((char*)vector->data + (vector->allocated * vector->type_size), &data, vector->type_size); \
	vector->allocated++; \
 \
	return true; \
}

JC_C_VECTOR_FUNCTION_DEFINITION_ALL_TYPES(JC_VECTOR_PUSHBACK_FUNCTION)



// All JC_vector_at macros. Call macro, definition macro, and all types generation macro

#define JC_vector_at(vector, type) _Generic((type), JC_C_VECTOR_GENERIC_ALL_TYPES(JC_vector_at))(vector, type)

#define JC_VECTOR_AT_FUNCTION(data_type, data_type_string) data_type JC_vector_at_##data_type_string(const Vector* const restrict vector, const size_t index) {\
	if (index >= vector->allocated) {\
		return 0;\
	}\
\
return *((data_type *)vector->data + index);\
}

JC_C_VECTOR_FUNCTION_DEFINITION_ALL_TYPES(JC_VECTOR_AT_FUNCTION)








#endif