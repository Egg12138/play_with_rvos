#include "types.h"
// #include "defs.h", 用的函数少，先不搞分离了。
#define LEN 8
extern void page_init(void);
extern void uart_init(void);
extern void uart_puts(char *s);
extern void uart_putstr(char *s);
extern char uart_getc();
extern void *uart_gets(char *str, uint8_t length);
extern int uart_putc(char c);
extern void schedule(void);
extern void sched_init(void);
extern void os_main(void);
extern void trap_init(void);
/*
 * 1. 初始化
 * 2. 读写 
 * */
void start_kernel(void)

{
	uart_init();
	char strbuf[50];
	char ch1 = uart_getc();
	uart_putstr("Hello, RVOS!\n");
    page_init();
    page_test();
    trap_init();
    uart_puts("Loop...");
    sched_init();
    uart_puts("schedule finished...\n");
    os_main();
    uart_puts("Task creation are all done!\n");
    schedule();
    
	while (1) {}

}
