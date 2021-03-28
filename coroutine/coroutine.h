#include <stdlib.h>
#include <time.h>

static clock_t millis;

enum coroutine_state {
	COROUTINE_BEGIN,
	COROUTINE_END,
	COROUTINE_ROOT
};

struct coroutine {
	int state;
	void (*func)(struct coroutine *);
	struct coroutine *prev;
	struct coroutine *next;
};

static struct coroutine root_coroutine = {COROUTINE_ROOT, NULL, NULL, NULL};

#define define_coroutine(name, body)					\
	void coroutine_func_##name(struct coroutine *self);		\
									\
	static struct coroutine name =					\
		{COROUTINE_BEGIN, coroutine_func_##name, NULL, NULL};	\
	void coroutine_func_##name(struct coroutine *self)		\
	{								\
		enum {							\
			CASE_LABEL_OFFSET = 				\
			((__COUNTER__ + 1) / 2) + COROUTINE_ROOT	\
		};							\
									\
		switch(name.state) {					\
		case COROUTINE_BEGIN:					\
			body						\
									\
		default:						\
			break;						\
		}							\
		name.state = COROUTINE_END;				\
	}

void insert_coroutine(struct coroutine *target)
{
	target->prev = &root_coroutine;
	target->next = root_coroutine.next;
	
	if (root_coroutine.next)
		root_coroutine.next->prev = target;
		
	root_coroutine.next = target;
}

void suspend_coroutine(struct coroutine *target)
{
	if (target->next)
		target->next->prev = target->prev;

	if (target->prev)
		target->prev->next = target->next;
}

void remove_coroutine(struct coroutine *target)
{
	suspend_coroutine(target);
	target->state = COROUTINE_BEGIN;
}

void resume_coroutine(struct coroutine *target)
{
	insert_coroutine(target);
}

void reset_coroutine(struct coroutine *target)
{
	target->state = COROUTINE_BEGIN;
}

#define interrupt_coroutine {						\
									\
		self->state =						\
			((__COUNTER__ + 1) / 2) - CASE_LABEL_OFFSET;	\
			return;						\
		case ((__COUNTER__ + 1) / 2) - CASE_LABEL_OFFSET - 1:;	\
}

#define delay_coroutine(value) {					\
	static clock_t delay;						\
	delay = millis;							\
									\
	while (millis - delay < value)					\
		interrupt_coroutine;					\
}
	
void update_coroutines()
{
	struct coroutine *iterator = root_coroutine.next;
	millis = clock() / (CLOCKS_PER_SEC/1000);
	
	while (iterator) {
		iterator->func(iterator);
		iterator = iterator->next;
	}
}
