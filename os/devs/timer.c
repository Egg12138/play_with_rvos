#include "os.h"

#define TIMER_INTERVAL CLINT_TIMEBASE_FREQ
static uint32_t _tick = 0;

void timer_interval_load(int interval)
{
	int id = r_mhartid();
	*(uint64_t *)CLINT_MTIMECMP(id) = *(uint64_t *)CLINT_MTIME + interval;
}

void timer_init()
{
	timer_interval_load(TIMER_INTERVAL);
	w_mie(r_mie() | MIE_MTIE);
	enable_global_interrupts();
}

void timer_interrupt_handler() 
{
	_tick++;
	printf("%d\n! ", _tick);
	timer_interval_load(TIMER_INTERVAL);
}

