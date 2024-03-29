# JC_C_Vector by Joshua Comer

A C implementation of the C++ std::vector which tries to remain as standards conforming with C++ as possible

The names of everything have been prefaced in such a way that there shouldn't be any naming collisions with anything else
As everything is at least preceeded by JC_

Why does this exist? I felt like it. Was a fun thing to write


Future Plans
------------

As of now the functions all take and return pointers as well so as to remain type independent

**In the future this is planned to support**
1) Non-pointer usage of the default built in types (e.g. being able to push_back an immediate of 8, rather than having to pass a pointer to a variable containing 8), and function overloading (this will require heavy use of macros and can only be done on C11 or beyond)
	It should be made trivial to add user defined structs for non pointer usage as well once this is finished. All that would be needed is simply adding your struct to the .h file and the neccessarry macros (All operations on the struct data contained within the vector will be shallow though)
2) Object Oriented Style and Storage of Pointers
	This will be done by several pointer functions (initialize, construct, destruct, move, compare, etc.). This will enable all of the same functionality for pointers which point to other data, especially if dynamically allocated, as regular types as long as the function pointers are set appropriately

Once both of those requirements are met this C implementation of the C++ std::vector should be as close as possible to the C++ standard


Functions Are Detailed Below
-----------

**Notes:**

All functions taking a JC_Vector* pointer assume that it is valid, and do not check if it's a NULL pointer

All functions which grow the vector (except for reserve) do so by multiplying the current vector size by JC_C_VECTOR_RESIZE_FACTOR

Some functions have been renamed, and all are detailed below. The most notable change however is push_back and insert to push_back_ptr and insert_ptr
The syntax change was to make it clear that the values inserted need to be a pointer, and also to allow for later additions to take up the push_back and insert function names

The erase and erase_if functions take an index, and not an iterator as a parameter

In addition, some functions have been added simply to mirror the C++ standard, when it's just as easy to access the variable from the JC_Vector struct directly
	e.g. JC_vector_size() simply returns vector->allocated



Setup and Cleanup
-----------------

**JC_Vector\* JC_vector_construct(size_t size, size_t type_size)**

* Dynamically creates a JC_Vector and returns a pointer to it. Size will be set to JC_C_VECTOR_MIN_ELEMENTS if size is smaller than that
* Possible Errors: Will return NULL if either malloc fails, or the total size of the request vector is greater than JC_C_VECTOR_MAX_SIZE


**void JC_vector_destruct(JC_Vector\*\* const restrict vector)**
	
* Frees the JC_Vector as well as the data contained within it
*	Possible Errors: None. Will return without effect if a NULL pointer is passed or a pointer to a NULL vector is passed



Element Access
--------------

**char\* JC_vector_at_ptr(const JC_Vector\* const restrict vector, const size_t index)**
* Returns a pointer to the memory location for the index provided. The pointer must be cast if any type other than char was contained within the vector
* Possible Errors: Will return NULL if the requetsed index is greater than the amount of accessible elements stored


**char\* JC_vector_at_ptr_unsafe(const JC_Vector\* const restrict vector, const size_t index)**
* Same as JC_vector_at_ptr() except there is no bounds checking.
* Possible Errors: UNSAFE - May crash the program if it attempts to access memory out of bounds


**char\* JC_vector_front(const JC_Vector\* const restrict vector)**
* Returns a pointer to the first element of the vector
* Possible Errors: Will return NULL if there are no elements within the vector
	

**char\* JC_vector_back(const JC_Vector\* const restrict vector)**
* Returns a pointer to last element of the vector
* Possible Errors: Will return NULL if there are no elements within the vector


**char\* JC_vector_data(const JC_Vector\* const restrict vector)**
* returns a pointer to the vector data. The same as accessing it via vector->data
* Possible Errors: Will return NULL if vector->data is set to NULL



Iterators
---------

**char\* JC_vector_begin(const JC_Vector\* const restrict vector)**
* Returns a pointer to the beginning of vector data. Essentially the same as accessing it via vector->data
* Possible Errors: Will be NULL if vector->data is NULL


**const char\* JC_vector_cbegin(const JC_Vector\* const restrict vector)**
* Same as above except the returned pointer is typed as const char *
* Possible Errors: Same as above


**char\* JC_vector_end(const JC_Vector\* const restrict vector)**
* Returns a pointer to one element past the vector
* Possible Errors: Could cause a program crash if dereferenced


**const char\* JC_vector_cend(const JC_Vector\* const restrict vector)**
* Same as above except the returned pointer is typed as const char *
* Possible Errors: Same as above


**Note: Due to no operator overloading in C there can't be rbegin or rend functions**



Capacity
--------

**bool JC_vector_empty(const JC_Vector\* const restrict vector)**
* Returns true if the vector is empty, false if not. The same as !vector->allocated
* Possible Errors: None


**size_t JC_vector_size(const JC_Vector\* const restrict vector)**
* returns the amount of elements within the vector. The same as vector->allocated
* Possible Errors: None


**size_t JC_vector_max_size()**
* returns the maximum size a vector can be. The same as using the macro JC_C_VECTOR_MAX_SIZE
* Possible Errors: None


**bool JC_vector_reserve(JC_Vector\* const restrict vector, const size_t size)**
* Grows the vector be able to contain size elements of the type size specified by the vector. Simply returns if the requested size is smaller than the vector's current capacity
* Possible Errors: Will return false if requested size is more than JC_C_VECTOR_MAX_SIZE or if malloc fails. In either case the vector will remain unchanged


**size_t JC_vector_capacity(const JC_Vector\* const restrict vector)**
* Returns the amount of elements, of the type size specified by the vector, which can be contained without growing the vector further. The same as vector->capacity
* Possible Errors: None


**bool JC_vector_shrink_to_fit(JC_Vector\* restrict vector)**
* Shrinks the amount of memory used by the vector, to just enough for all elements currently inside of it. Will not shrink below an element size of 1
* Possible Errors: Will return false if realloc fails. The vector will be unchanged in this case



Modifiers
---------

**void JC_vector_clear(JC_Vector\* const restrict vector)**
* Clears the vector of all elements contained. Does not alter the amount of memory used by the vector at all. For that use JC_vector_shrink_to_fit() after JC_vector_clear()
* Possible Errors: None


**void\* JC_vector_insert_ptr(JC_Vector\* const restrict vector, const size_t index, const void\* const restrict value)**
* Inserts value into the vector at the index specified. All elements at index or greater are moved to an index 1 higher before the insertion in order to make room for value. Meaning the vector might grow if needed. Returns a pointer to the inserted value. Value is assumed to be the same type_size as the type_size set for the vector
* Possible Errors: Returns NULL if either the index is out of bounds, or if inserting would require the vector grows and that growing failed. In either case the vector is unchanged


**char\* JC_vector_erase(JC_Vector\* const restrict vector, const size_t index)**
* Removes an element from the vector at the index specified. All items above the index are moved to 1 index lower to keep the vector contiguous. Returns a pointer to the erased position
* Possible Errors: Will return NULL if the index is out of bounds


**bool JC_vector_pushback_ptr(JC_Vector\* const restrict vector, const void\* const restrict data)**
* Pushes the data provided onto the end of the vector, increasing its length by 1 element. The vector grows if there is not enough space for the data provided
* Possible Errors: Returns false if the vector needs to grow, and that growing fails. The vector is unchanged in this case


**inline void JC_vector_pop_back(JC_Vector\* const restrict vector)**
* Removes an element from the end of the vector, decreasing its length by 1 element. In the event that the vector is empty, nothing happens
* Possible Errors: None


**bool JC_vector_resize(JC_Vector\* const restrict vector, const size_t new_size)**
* Resizes the vector to contain new_size elements. If new_size is less than the current amount of elements, it simply shrinks. If new_size is larger than the current amount of elements within vector, the vector increases in size (growing if neccessary) to contain new_size elements. All new elements from this size increase are initialized to 0. If you wish to increase the size of the vector without initialization, use the JC_vector_reserve() function
* Possible Errors: Returns false if the vector needs to grow, and that growing fails


**bool JC_vector_resize_ptr(JC_Vector\* const restrict vector, const size_t new_size, const void\* const restrict default_value)**
* The same as the function directly above, except that instead of new values being initialized to 0, they are intialized to the default_value provided. default_value is assumed to be the same size as vector's type_size
* Possible Errors: Returns false if the vector needs to grow, and that growing fails


**void JC_vector_swap(JC_Vector\*\* const restrict vector1, JC_Vector\*\* const restrict vector2)**
* Swaps the data between the two vectors
* Possible Errors: None



Other Functions
---------------

**bool JC_vector_are_same_shallow(JC_Vector\* const restrict vector1, JC_Vector\* const restrict vector2)**
* Compares 2 vectors, and returns true if the memory values contained within them are the same, returns false if otherwise. Named shallow because if any pointers are contained they are not dereferenced
* Possible Errors: None


**int JC_vector_erase_if_same(JC_Vector\* const restrict vector, const void\* const restrict value)**
* Compares each element contained within the vector, and erases any elements which are the same as the value provided. Returns the number of erased elements
* Possible Errors: None


**int JC_vector_erase_if_predicate(JC_Vector\* const restrict vector, bool predicate_function())**
* Passes each element contained within the vector to the function pointer provided. If true is returned by the function then the element is erased. Returns the number of elements erased
* Possible Errors: None, however make sure that the function pointer expects a single parameter. A pointer of the type contained within the vector



Debug Functions
---------------
	
**void JC_vector_safe_dump(const JC_Vector\* const restrict vector)**
* Prints out every element within the vector's bounds. The type is assumed to be int by default. Simply change the pointer type and print statement if there needs to be any difference
* Possible Errors: None assuming the print statement and the pointer within the function is typed correctly


**void JC_vector_unsafe_dump(const JC_Vector\* const restrict vector)**
* The same as above, except that it prints every element within the vector's capacity, or its available memory. This includes elements which are not within the vector's bounds
* Possible Errors: None assuming the print statement and the pointer within the function is typed correctly
