// trap.c
#include "os.h"
#define IS_INTERUPTION(x) (x&0x80000000)
extern void trap_vector(void);

void trap_init(void) {
	w_mtvec((reg_t)trap_vector);
}

reg_t trap_handler(reg_t epc, reg_t cause) {

	reg_t return_pc = epc;
	reg_t cause_code = cause & 0xfff;

	if IS_INTERUPTION(cause) {
		switch (cause_code) {
		case 1: uart_puts("Supervisor software interrupt\n");break;
		case 3: uart_puts("Machine software interrupt!\n"); break;
		case 5: uart_puts("Supervisor timer interrupt!\n"); break;
		case 7: uart_puts("Machine timer interrupt!\n"); break;
		case 9: uart_puts("Supervisor external interrupt!\n"); break;
		case 11: uart_puts("Machine external interrupt!\n"); break;
		default: uart_puts("unknown async exception!\n"); break;
		}		
	} else {
		printf("Sync expetions. Exception Code is %d\n", cause_code);
		panic("OOPS! What can I do!");
		//return_pc += 4;
	} 
	return return_pc;
}


void trap_test()
{
	/*
	 * Synchronous exception code = 7
	 * Store/AMO access fault
	 */
	*(int *)0x00000000 = 100;

	/*
	 * Synchronous exception code = 5
	 * Load access fault
	 */
	//int a = *(int *)0x00000000;

	uart_puts("Back from the trap!\n");
}


