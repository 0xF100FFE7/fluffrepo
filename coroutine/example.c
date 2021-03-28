#include "coroutine.h"

#include <stdio.h>
define_coroutine(async_counter,
{
	static int i = 0;
	
	while (1) {	
		delay_coroutine(1000);
		printf("async counter: %i\n", i++);
		interrupt_coroutine;
		printf("1: %i\n", i++);
		interrupt_coroutine;
		printf("2: %i\n", i++);
		interrupt_coroutine;
		printf("3: %i\n", i++);
		interrupt_coroutine;
		printf("4: %i\n", i++);
		interrupt_coroutine;
		fflush(NULL);
	}
})

int main()
{
	insert_coroutine(&async_counter);
	
	while (1) {
		update_coroutines();
	}
	
	return 0;
}
