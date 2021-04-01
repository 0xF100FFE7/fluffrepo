# C89 coroutines

**This is minimal (under 150 lines of code), non-reenterant, coroutine implementation on ANSI C language.**

- Coroutines are a double-linked list;

- You can define the function of a coroutine using **define_coroutine**;

- Add a new coroutine to a linked list with **insert_coroutine**;

- Remove with: **remove_coroutine**;

- All operations are performed in a constant amount of time;

- You can interrupt execution of a coroutine and start execution of another one with **interrupt_coroutine**;

- The function **delay_coroutine** interrupts execution of the coroutine until a certain time;

- The function **suspend_coroutine** removes the coroutine from the list, while preserving the state of the coroutine;

- The **resume_coroutine** function inserts the coroutine back in;

- The function **reset_coroutine** resets the state of the coroutine to its original state;

- The function **update_coroutines** goes through the entire list of coroutines and executes them;



*The coroutines themselves are switch-case automata wrapped in macros. Each interrupt call inserts a new case label into the code, to which it then returns after the interrupt.*

*And all this is packed into literally 100 lines in the oldest language that does not know stl.*


**Example of usage:**
```C
#include <stdio.h>
#include "coroutine.h"

define_coroutine(async_counter,
{
	static int i = 0;
	
	while (1) {	
		printf("async counter: %i\n", i++);
		fflush(NULL);
		delay_coroutine(1000);
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
```
