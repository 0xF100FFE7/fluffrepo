#ifndef COROUTINE_GUARD
#define COROUTINE_GUARD

#include <stdlib.h>
#include <time.h>

#ifndef COROUTINE_CLOCK

/* Time from start in milliseconds*/
#define COROUTINE_CLOCK clock() / (CLOCKS_PER_SEC/1000)
#endif

static clock_t millis;

enum coroutine_states {COROUTINE_BEGIN, COROUTINE_END};

struct coroutine {
	int state;
	
	/* Pointer to coroutine function */
	void (*func)(struct coroutine *);
	
	struct coroutine *prev;
	struct coroutine *next;
};

/* Head node of the list. Not used as actual coroutine */
static struct coroutine root_coroutine = {COROUTINE_BEGIN, NULL, NULL, NULL};

#define DEFINE_COROUTINE(name, body)					\
	/* prototype of coroutine function */				\
	void coroutine_func_##name(struct coroutine *);			\
									\
	/* new coroutine initializes with pointer to that function */	\
	static struct coroutine name =					\
		{COROUTINE_BEGIN, coroutine_func_##name, NULL, NULL};	\
									\
	/* actual definition */						\
	void coroutine_func_##name(struct coroutine *self)		\
	{								\
		/* It saves last generated __COUNTER__ + offset */	\
		enum {LAST_COUNTER = __COUNTER__ + COROUTINE_END};	\
									\
		switch(self->state) {					\
		case COROUTINE_BEGIN:					\
			body						\
									\
		case COROUTINE_END:					\
		default:						\
			break;						\
									\
		/* Here we will insert generated case statements */	\
									\
		}							\
									\
		/* Idle if our coroutine reached end of execution */	\
		self->state = COROUTINE_END;				\
	}

/* Inserts coroutine in root node if not already inserted */
void insert_coroutine(struct coroutine *target)
{
	if (!target->prev) {
		target->prev = &root_coroutine;
		target->next = root_coroutine.next;
		
		if (root_coroutine.next)
			root_coroutine.next->prev = target;
			
		root_coroutine.next = target;
	}
}

/* Removes coroutine from root if not already removed.
 * Not restoring coroutine state */
void suspend_coroutine(struct coroutine *target)
{	
	if (target->prev) {
		target->prev->next = target->next;
		if (target->next)
			target->next->prev = target->prev;
		target->prev = NULL;
	}
}

/* Restores coroutine into its initial state */
void reset_coroutine(struct coroutine *target)
{
	target->state = COROUTINE_BEGIN;
}

/* Removes coroutine from root if not already removed.
 * Restores coroutine into its initial state */
void remove_coroutine(struct coroutine *target)
{
	suspend_coroutine(target);
	reset_coroutine(target);
}

/* Inserts coroutine into the root node again
 * Keep working from the last saved state */
void resume_coroutine(struct coroutine *target)
{
	insert_coroutine(target);
}

#define YIELD_COROUTINE {						\
	/* Generate new case label after COROUTINE_END */		\
	enum {GENERATED_CASE = LAST_COUNTER - __COUNTER__};		\
									\
	/* Save this label in our state */				\
	self->state = GENERATED_CASE;					\
									\
	/* Leave our coroutine */					\
	return;								\
									\
	/* Start execution here after we back */			\
	case GENERATED_CASE:;						\
}

#define DELAY_COROUTINE(value) {					\
	static clock_t _delay;						\
	_delay = millis;						\
									\
	while (millis - _delay < value)					\
		YIELD_COROUTINE;					\
}

/* Goes throught all coroutines in list and executes them */ 
void update_coroutines()
{
	struct coroutine *iterator = root_coroutine.next;
	millis = COROUTINE_CLOCK;
	
	while (iterator) {
		iterator->func(iterator);
		iterator = iterator->next;
	}
}

#endif
