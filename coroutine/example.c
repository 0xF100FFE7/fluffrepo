#include <stdio.h>
#include "coroutine.h"

DEFINE_COROUTINE(async_counter1,
{
	static int i = 0;
	static int delay = 1337;
	
	while (1) {
		if (delay >= 232)
			delay -= 100;
		else
			delay = 1337;
			
		printf("@@@@@@@@@@ ASYNC COUNTER1 @@@@@@@@@@: %i\n", i++);
		fflush(NULL);
		DELAY_COROUTINE(delay);
	}
})

DEFINE_COROUTINE(async_counter2,
{
	static int i = 0;

	while (1) {
		printf("__________ ASYNC COUNTER2 __________: %i\n", i++);
		YIELD_COROUTINE;
		fflush(NULL);
		DELAY_COROUTINE(232);
	}
})

int main()
{
	insert_coroutine(&async_counter1);
	insert_coroutine(&async_counter2);

	while (1) {
		update_coroutines();
	}
	
	return 0;
}
