#include "os.h"

extern void trap_vector(void);

void trap_init()
{
	/*
	 * set the trap-vector base-address for machine-mode
	 */
	w_mtvec((reg_t)trap_vector);
}
void external_interrupt_handler()
{
	int irq = plic_claim();
	if (irq == UART0_IRQ) {
		uart_isr();
	} else if (irq) {
		printf("Unexpected interrupt irq = %d\n", irq);
	}
	if (irq) {
		plic_complete(irq);
	}
}


reg_t trap_handler(reg_t epc, reg_t cause)
{
	reg_t return_pc = epc;
	reg_t cause_code = cause & 0xfff;
	
	if (cause & 0x80000000) {
		/* Asynchronous trap - interrupt */
		switch (cause_code) {
		case 3:
			uart_puts("software interruption!:");
			printf("mcause: %dL, epc: %d\n", cause, return_pc);
			break;
		case 7:
			// uart_puts("timer interruption!\n");
			uart_puts("clock: ");
			timer_interrupt_handler();
			break;
		case 11:
			printf("[%lu], epc: %lu\n", cause_code, return_pc);
			external_interrupt_handler();
			break;
		default:
			uart_puts("unknown async exception!\n");
			break;
		}
	} else {
		/* Synchronous trap - exception */
		printf("Sync exceptions!, code = %d\n", cause_code);
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

	uart_puts("Yeah! I'm back from the trap!\n");
}

