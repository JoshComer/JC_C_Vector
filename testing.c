#include <stdio.h>
#include "JC_C_Vector.h"
#include <assert.h>

#define INITIALIZE_TEST_NUM 42

typedef struct test_struct {
	int num;
	int num_doubled;
	int num_squared;
} test_struct;

bool erase_test_if_even(test_struct* data)
{
	return data->num % 2 == 0;
}

bool erase_test_if_odd(test_struct* data)
{
	return data->num % 2 == 1;
}


bool construct_test()
{

	{
		JC_Vector * vec = JC_vector_construct(0, 0);

		assert(JC_vector_empty(vec));

		assert(vec->capacity == JC_C_VECTOR_MIN_ELEMENTS);
		assert(JC_vector_capacity(vec) == vec->capacity);
		assert(vec->allocated == 0);
		assert(JC_vector_size(vec) == vec->allocated);
		assert(vec->type_size == 0);

		assert(vec->data != NULL);

		JC_vector_destruct(&vec);
		assert(vec == NULL);
	}

	{
		JC_Vector* vec = JC_vector_construct(50, sizeof(int));
		
		assert(JC_vector_empty(vec));

		assert(vec->capacity == 50);
		assert(JC_vector_capacity(vec) == vec->capacity);
		assert(vec->allocated == 0);
		assert(JC_vector_size(vec) == vec->allocated);
		assert(vec->type_size == sizeof(int));
	
		assert(vec->data != NULL);

		JC_vector_destruct(&vec);
	}

	{
		JC_Vector * vec = JC_vector_construct(30, sizeof(test_struct));

		assert(JC_vector_empty(vec));

		assert(vec->capacity == 30);
		assert(JC_vector_capacity(vec) == vec->capacity);
		assert(vec->allocated == 0);
		assert(JC_vector_size(vec) == vec->allocated);
		assert(vec->type_size == sizeof(test_struct));

		assert(vec->data != NULL);

		JC_vector_destruct(&vec);
	}

	return true;

}


bool push_and_pop_test()
{
	{
		JC_Vector* vec = JC_vector_construct(40, sizeof(int));

		// pushback
		for (int i = 0; i < vec->capacity; i++)
		{
			JC_vector_pushback_ptr(vec, &i);
		}

		assert(!JC_vector_empty(vec));
		assert(vec->allocated == vec->capacity);
		assert(JC_vector_size(vec) == JC_vector_capacity(vec));

		// check push_back with at
		for (int i = 0; i < vec->capacity; i++)
		{
			assert(i == *(int *)JC_vector_at_ptr(vec, i));
		}

		// pop_back and check as we go
		for (int i = vec->allocated - 1; i >= 0; i--)
		{
			assert(*(int*)JC_vector_at_ptr(vec, i) == i);
			JC_vector_pop_back(vec);
		}

		assert(vec->allocated == 0);
		assert(JC_vector_empty(vec));
		
		// pop_back on an empty vector is undefined by C++ standards, but for this implementation the function simply does nothing if the vector is empty
		JC_vector_pop_back(vec);
		assert(vec->allocated == 0);

		// at returns NULL if the vector is empty
		assert(JC_vector_at_ptr(vec, 1) == NULL);

		// at_unsafe will enable you to index into data past the official bounds if you so desire (undefined by C++ standards). No official guarantee is made for safety, however this implementation will generally not touch data that got popped
		// there is a guarantee made however that indexing below capacity will not give an error
		for (int i = 0; i < vec->capacity; i++)
		{
			assert(*(int*)JC_vector_at_ptr_unsafe(vec, i) == i);
		}

		JC_vector_destruct(&vec);
	}


	return true;

}


bool grow_and_shrink_test()
{
	{
		JC_Vector* vec = JC_vector_construct(50, sizeof(int));
		assert(vec->capacity == 50);

		assert(JC_vector_reserve(vec, 100));
		assert(vec->capacity == 100);

		assert(JC_vector_reserve(vec, 500));
		assert(vec->capacity == 500);

		assert(JC_vector_empty(vec));
		for (int i = 0; i < vec->capacity; i++)
		{
			JC_vector_pushback_ptr(vec, &i);
		}
		assert(!JC_vector_empty(vec));

		assert(vec->capacity == 500);
		assert(vec->allocated == 500);

		int temp = 500;
		JC_vector_pushback_ptr(vec, &temp);

		assert(vec->capacity == 500 * JC_C_VECTOR_RESIZE_FACTOR);
		assert(vec->allocated == 501);


		for (int i = 0; i < vec->allocated; i++)
		{
			assert(*(int*)JC_vector_at_ptr(vec, i) == i);
		}

		assert(JC_vector_shrink_to_fit(vec));
		assert(vec->capacity == vec->allocated);

		for (int i = 0; i < vec->allocated; i++)
		{
			assert(*(int*)JC_vector_at_ptr(vec, i) == i);
		}

		JC_vector_destruct(&vec);
	}

	return true;
}


bool insert_test()
{
	{
		JC_Vector* vec = JC_vector_construct(20, sizeof(int));
		assert(vec->capacity == 20);
		assert(vec->allocated == 0);
		assert(JC_vector_empty(vec));

		for (int i = 0; i < 10; i++)
			assert(*(int *)JC_vector_insert_ptr(vec, i, &i) == i);

		for (int i = 0; i < 10; i++)
			assert(*(int *)JC_vector_at_ptr(vec, i) == i);
		

		// insert -42 20 times in the middle of vec, causing it to grow. Also insert once at the beginning and once at the end as well
		int insert_val = -42;
		int middle_insert_index = 8;
		int num_inserts = 22;
		int num_inserts_in_middle = 20;
		
		assert(*(int*)JC_vector_insert_ptr(vec, 0, &insert_val) == insert_val);

		for (int i = 0; i < num_inserts_in_middle; i++)
			assert(*(int*)JC_vector_insert_ptr(vec, middle_insert_index, &insert_val) == insert_val);

		assert(*(int*)JC_vector_insert_ptr(vec, vec->allocated, &insert_val) == insert_val);

		// check that insertions worked properly. Should be insert val at 0, insert val at the end, 0-9 ascending, and num_inserts amount of insert_val at num_inserts_in_middle interrupting the ascending 0-9
		assert(vec->capacity == 20 * JC_C_VECTOR_RESIZE_FACTOR);
		assert(vec->allocated == 10 + num_inserts);
		
		assert(*(int*)JC_vector_at_ptr(vec, 0) == insert_val);


		for (int i = 1, check_num = 0; i < middle_insert_index; i++, check_num++)
			assert(*(int*)JC_vector_at_ptr(vec, i) == check_num);

		for (int i = middle_insert_index; i < middle_insert_index + num_inserts_in_middle; i++)
			assert(*(int*)JC_vector_at_ptr(vec, i) == insert_val);

		for (int i = middle_insert_index + num_inserts_in_middle, check_num = middle_insert_index - 1; i < vec->allocated - 1; i++, check_num++)
			assert(*(int *)JC_vector_at_ptr(vec, i) == check_num);


		assert(*(int*)JC_vector_at_ptr(vec, vec->allocated - 1) == insert_val);

		
		JC_vector_destruct(&vec);
	}

	return true;
}


bool erase_test()
{
	{
		JC_Vector* vec = JC_vector_construct(40, sizeof(char));


		for (char i = 0; i < 41; i++)
			JC_vector_insert_ptr(vec, i, &i);
		

		assert(vec->allocated == 41);

		// relies on unsafe behavior of the fact that the value will be left over in memory even though it's been "erased" from the end
		assert(*(char*)JC_vector_erase(vec, vec->allocated - 1) == 40);
		assert(vec->allocated == 40);


		for (char i = 0; i < 40; i++)
			assert(*(char*)JC_vector_at_ptr(vec, i) == i);


		// for erasing everything but the end like above, the returned pointer is for the data that was immediately after it
		assert(*(char*)JC_vector_erase(vec, vec->allocated - 2) == 39);
		assert(*(char*)JC_vector_erase(vec, 0) == 1);

		while (vec->allocated != 0)
		{
			JC_vector_erase(vec, 0);
		}

		JC_vector_destruct(&vec);

	}

	// tests that the entire vector after erase
	{
		JC_Vector* vec = JC_vector_construct(80, sizeof(unsigned short));


		for (unsigned short i = 0; i < 82; i++)
			JC_vector_pushback_ptr(vec, &i);


		// because we are constantly erasing the second to last element, the returned pointer will always point to the last element
		for (unsigned short i = 80; i > 0; i--)
			assert(*(unsigned short*)JC_vector_erase(vec, i) == 81);


		assert(*(unsigned short*)JC_vector_erase(vec, 1) == 81);
		assert(vec->allocated == 1);
		assert(*(unsigned short*)JC_vector_erase(vec, 0) == 0);
		
		assert(JC_vector_empty(vec));




		// retests the empty vector by simply growing it, and erasing everything
		for (unsigned short i = 0; i < 900; i++)
			JC_vector_pushback_ptr(vec, &i);


		for (unsigned short i = 1; i < 900; i++)
			assert(*(unsigned short *)JC_vector_erase(vec, 0) == i);	


		assert(!JC_vector_empty(vec));
		assert(*(unsigned short*)JC_vector_erase(vec, 0) == 899);
		assert(JC_vector_empty(vec));


		JC_vector_destruct(&vec);
	}

	return true;
}


bool erase_if_test()
{
	// erase if test
	{
		JC_Vector* vec = JC_vector_construct(40, sizeof(int));


		for (int i = 0; i < 40; i++)
			JC_vector_pushback_ptr(vec, &i);

		int erase_num = 6;

		JC_vector_insert_ptr(vec, 0, &erase_num);
		JC_vector_insert_ptr(vec, vec->allocated, &erase_num);

		for (int i = 0; i < 3; i++)	
			JC_vector_insert_ptr(vec, 27, &erase_num);




		assert(JC_vector_erase_if_same(vec, &erase_num) == 6);

		for (int i = 0; i < erase_num; i++)
			assert(*(int *)JC_vector_at_ptr(vec, i) == i);

		// checks for i + 1 past erase_num since we erased erase_num in the middle of vec
		for (int i = erase_num; i < vec->allocated; i++)
			assert(*(int*)JC_vector_at_ptr(vec, i) == i + 1);


		JC_vector_destruct(&vec);
	}

	// erase_if_predicate and are_same_shallow test
	{
		JC_Vector* vec1 = JC_vector_construct(50, sizeof(test_struct));
		JC_Vector* vec2 = JC_vector_construct(50, sizeof(test_struct));

		for (int i = 0; i < 50; i++)
		{
			test_struct temp_data;
			temp_data.num = i;
			temp_data.num_doubled = i * 2;
			temp_data.num_squared = i * i;

			JC_vector_pushback_ptr(vec1, &temp_data);
			JC_vector_pushback_ptr(vec2, &temp_data);
		}

		assert(JC_vector_are_same_shallow(vec1, vec2));

		JC_vector_erase_if_predicate(vec1, erase_test_if_even);
		JC_vector_erase_if_predicate(vec2, erase_test_if_odd);

		assert(!JC_vector_are_same_shallow(vec1, vec2));

		for (int i = 0; i < 50; i++)
		{
			// evens were erased from vec, odds were erased from vec2
			if (i % 2 == 0)
			{
				// check vec2 when it's even numbers
				test_struct* temp_data = JC_vector_at_ptr(vec2, i / 2);
				assert(temp_data->num == i);
				assert(temp_data->num_doubled == i * 2);
				assert(temp_data->num_squared == i * i);
			}
			else
			{
				// check vec1 when it's odd numbers
				test_struct* temp_data = JC_vector_at_ptr(vec1, i / 2);
				assert(temp_data->num == i);
				assert(temp_data->num_doubled == i * 2);
				assert(temp_data->num_squared == i * i);
			}
		}

		JC_vector_destruct(&vec1);
		JC_vector_destruct(&vec2);
	}

	return true;
}


bool are_same_test() 
{
	// some edge case for are_same tests. Also uses iterators as well
	{
		// Test if element is different at begging
		{
			JC_Vector* vec1 = JC_vector_construct(5, sizeof(int));
			JC_Vector* vec2 = JC_vector_construct(5, sizeof(int));

			for (int i = 0; i < 5; i++)
			{
				JC_vector_pushback_ptr(vec1, &i);
				JC_vector_pushback_ptr(vec2, &i);
			}

			assert(JC_vector_are_same_shallow(vec1, vec2));

			*(int*)JC_vector_begin(vec2) = -99;
			assert(*(int*)JC_vector_at_ptr(vec2, 0) == -99);

			assert(!JC_vector_are_same_shallow(vec1, vec2));

			JC_vector_destruct(&vec1);
			JC_vector_destruct(&vec2);
		}

		// Test if element is different at end
		{
			JC_Vector* vec1 = JC_vector_construct(5, sizeof(int));
			JC_Vector* vec2 = JC_vector_construct(5, sizeof(int));

			for (int i = 0; i < 5; i++)
			{
				JC_vector_pushback_ptr(vec1, &i);
				JC_vector_pushback_ptr(vec2, &i);
			}

			assert(JC_vector_are_same_shallow(vec1, vec2));

			*((int*)JC_vector_end(vec2) - 1) = -99;
			assert(*(int*)JC_vector_at_ptr(vec2, vec2->allocated - 1) == -99);

			assert(!JC_vector_are_same_shallow(vec1, vec2));

			JC_vector_destruct(&vec1);
			JC_vector_destruct(&vec2);
		}

		// Test if one vector has an additional number
		{
			JC_Vector* vec1 = JC_vector_construct(5, sizeof(int));
			JC_Vector* vec2 = JC_vector_construct(5, sizeof(int));

			for (int i = 0; i < 5; i++)
			{
				JC_vector_pushback_ptr(vec1, &i);
				JC_vector_pushback_ptr(vec2, &i);
			}

			int push_num = -99;
			JC_vector_pushback_ptr(vec2, &push_num);

			assert(!JC_vector_are_same_shallow(vec1, vec2));

			JC_vector_destruct(&vec1);
			JC_vector_destruct(&vec2);
		}
	}

	return true;
}


bool swap_test()
{
	{
		JC_Vector* vec1 = JC_vector_construct(20, sizeof(int));
		JC_Vector* vec2 = JC_vector_construct(40, sizeof(char));

		for (int i = 0; i < 20; i++)
			JC_vector_pushback_ptr(vec1, &i);

		for (char i = 0; i < 40; i++)
			JC_vector_pushback_ptr(vec2, &i);

		
		assert(!JC_vector_are_same_shallow(vec1, vec2));

		JC_vector_swap(&vec1, &vec2);

		assert(vec1->allocated == 40);
		assert(vec1->type_size == sizeof(char));
		assert(vec2->allocated == 20);
		assert(vec2->type_size == sizeof(int));

		for (int i = 0; i < 20; i++)
			assert(*(int*)JC_vector_at_ptr(vec2, i) == i);

		for (char i = 0; i < 40; i++)
			assert(*JC_vector_at_ptr(vec1, i) == i);

		JC_vector_destruct(&vec1);
		JC_vector_destruct(&vec2);
	}

	return true;
}


bool resize_test()
{
	// test resizing with more values "default inserted" (set as 0)
	{
		JC_Vector* vec = JC_vector_construct(20, sizeof(int));

		JC_vector_resize(vec, 80);

		for (int i = 0; i < vec->allocated; i++)
			assert(*(int*)JC_vector_at_ptr(vec, i) == 0);

		JC_vector_destruct(&vec);
	}

	// test resizing with -99 as the new value
	{
		JC_Vector* vec = JC_vector_construct(20, sizeof(int));

		int insert_val = -99;
		JC_vector_resize_ptr(vec, 80, &insert_val);

		for (int i = 0; i < vec->allocated; i++)
			assert(*(int*)JC_vector_at_ptr(vec, i) == insert_val);

		JC_vector_destruct(&vec);
	}

	// test resizing to below what was there before
	{
		JC_Vector* vec = JC_vector_construct(20, sizeof(int));

		for (int i = 0; i < 100; i++)
			JC_vector_insert_ptr(vec, i, &i);

		JC_vector_resize(vec, 20);

		assert(vec->allocated == 20);
		
		for (int i = 0; i < 20; i++)
			assert(*(int*)JC_vector_at_ptr(vec, i) == i);

		JC_vector_destruct(&vec);
	}
}






int main() {
	
	assert(JC_vector_max_size() == JC_C_VECTOR_MAX_SIZE);

	// Tests for the first implementation. Type independant via pointers, and no object oriented support
	assert(construct_test());
	assert(push_and_pop_test());
	assert(grow_and_shrink_test());
	assert(insert_test());
	assert(erase_test());
	assert(erase_if_test());
	assert(are_same_test());
	assert(swap_test());
	assert(resize_test());

	return 0;
}
