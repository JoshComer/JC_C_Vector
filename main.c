#include <stdio.h>
#include "JC_C_Vector.h"

int main() {

	Vector vec = JC_vector_construct(20, sizeof(int));


	for (int i = 0; i < vec.capacity / 2; i++) {
		JC_vector_pushback_ptr(&vec, &i);
	}
	
	for (int i = 0; i < vec.capacity; i++) {
		printf("%d:%d\n", i, *(int *)JC_vector_at_ptr_unsafe(&vec, i));
	}

	printf("Hello World\n");

	
	int testing = 8;
	JC_vector_pushback(&vec, testing);

	for (int i = 0; i < vec.capacity; i++) {
		printf("%d:%d\n", i, JC_vector_at(&vec, i));
	}

	JC_vector_destruct(&vec);
}