#include "types.h"
#include "platform.h"

/*
 * The UART control registers are memory-mapped at address UART0. 
 * This macro returns the address of one of the registers(UART0 + reg偏移量得到寄存器的地址。解引用之从而访问。).
 *
 */
/* 已经是一个解引用了 */
#define UART_REG(reg) ((volatile uint8_t *)(UART0 + reg)) //reg 应该为 hex or ...
#define uart_read_reg(reg) (*(UART_REG(reg)))  		  //再对这个地址进行解引用。
#define uart_write_reg(reg, v) (*(UART_REG(reg)) = (v))   // write value into register 



/*
 * Reference
 * [1]: TECHNICAL DATA ON 16550, http://byterunner.com/16550.html
 */

/*
 * UART芯片提供了若干寄存器，
 * 一个宏描述的是一个寄存器偏移量, 
 @@ 一个REG的bit maps在td16..pdf可以看
A2A1A0REG.BIT 7BIT 6BIT 5BIT 4BIT 3BIT 2BIT 1BIT 0
000RHRbit 7bit 6bit 5bit 4bit 3bit 2bit 1bit 0
000THRbit 7bit 6bit 5bit 4bit 3bit 2bit 1bit 0

 *** UART control registers map**. see [1] "PROGRAMMING TABLE"
 * note some are reused by multiple functions
 * 0 (write mode): THR/DLL
 * 1 (write mode): IER/DLM
 */
/**
 * 我们会发现下面有些事重复的数字，再实际运行中我们还会有多一位来对两个功能（显然同时只能开启一个）进行区分
 */
#define RHR 0	// Receive Holding Register (read mode)
// THR/RBR存放或收取数据，现在的芯片的THR有FIFO机制的缓存空间
#define THR 0	// Transmit Holding Register (write mode)
// DLL, DLM 8bits, 8bits -> 16 bits. 设置baud rate
#define DLL 0	// LSB of Divisor Latch (write mode)
#define IER 1	// Interrupt Enable Register (write mode)
// IER中断管理寄存器的每个bit的意义不同，具体要看手册
#define DLM 1	// MSB of Divisor Latch (write mode)
#define FCR 2	// FIFO Control Register (write mode)
#define ISR 2	// Interrupt Status Register (read mode)
#define LCR 3	// Line Control Register
// LCR 可以设DLAB和模式
#define MCR 4	// Modem Control Register
// MCR设定硬件控制
#define LSR 5	// Line Status Register
#define MSR 6	// Modem Status Register
// MSR描述UART的状态，每一位的意义看手册
#define SPR 7	// ScratchPad Register

/*
 * POWER UP DEFAULTS(加电后默认值)
 * IER = 0: TX/RX holding register interrupts are both disabled
 * ISR = 1: no interrupt penting
 * LCR = 0
 * MCR = 0
 * LSR = 60 HEX
 * MSR = BITS 0-3 = 0, BITS 4-7 = inputs
 * FCR = 0
 * TX = High
 * OP1 = High
 * OP2 = High
 * RTS = High
 * DTR = High
 * RXRDY = High
 * TXRDY = Low
 * INT = Low
 */

/*
 * LINE STATUS REGISTER (LSR)
 * LSR BIT 0:
 * 0 = no data in receive holding register or FIFO.
 * 1 = data has been receive and saved in the receive holding register or FIFO.
 * ......
 * LSR BIT 5:
 * 0 = transmit holding register is full. 16550 will not accept any data for transmission.
 * 1 = transmitter hold register (or FIFO) is empty. CPU can load the next character.
 * ......
 */
#define LSR_RX_READY (1 << 0)
#define LSR_TX_IDLE  (1 << 5) // 4 bytes.


void uart_init()
{
	// 禁用中断
			// 等UART正常可用
	// 设置波特率
			// 设置串口模式
	// 设置奇偶校验
	// 数据呢？数据在init之后再传入
	/* disable interrupts. */
	uart_write_reg(IER, 0x00);

	/*
	 * Setting baud rate. Just a demo here if we care about the divisor,
	 * but for our purpose [QEMU-virt], this doesn't really do anything.
	 *
	 * Notice that the divisor register DLL (divisor latch least) and DLM (divisor
	 * latch most) have the same base address as the receiver/transmitter and the
	 * interrupt enable register. To change what the base address points to, we
	 * open the "divisor latch" by writing 1 into the Divisor Latch Access Bit
	 * (DLAB), which is bit index 7 of the Line Control Register (LCR).
	 *
	 * Regarding the baud rate value, see [1] "BAUD RATE GENERATOR PROGRAMMING TABLE".
	 * We use 38.4K when 1.8432 MHZ crystal, so the corresponding value is 3.
	 * And due to the divisor register is two bytes (16 bits), so we need to
	 * split the value of 3(0x0003) into two bytes, DLL stores the low byte,
	 * DLM stores the high byte.
	 */
	// 加电后的默认LCR=^见defines.
	/*
	 * 设定特定的值到寄存器中， 波特率会由这个寄存器的值生成
	 * [DLL, DLM]两个寄存器
	 */
	uint8_t lcr = uart_read_reg(LCR); // the return value is of 'uint8_t' 
	uart_write_reg(LCR, lcr | (1 << 7)); 
	// DL: 0x0003, 该值是我们计算出来的
	uart_write_reg(DLL, 0x03); // 低位
	uart_write_reg(DLM, 0x00); // 高位

	/* 奇偶校验
	 * Continue setting the asynchronous data communication format.
	 * - number of the word length: 8 bits
	 * - number of stop bits：1 bit when word length is 8 bits
	 * - no parity
	 * - no break control
	 * - disabled baud latch
	 */
	lcr = 0;
	uart_write_reg(LCR, lcr | (3 << 0));
}

int uart_putc(char ch)
{
	// Line Status: TX not busy and LSR == 1
	// call uart_putc之后，就一直轮询看LSR是否LSR是LSR_TX_IDLE==1的，是就是空闲.
	while (!(uart_read_reg(LSR) & LSR_TX_IDLE) );
	return uart_write_reg(THR, ch);
}

char uart_getc()
{
	// Check Line Status: 
	while (!(uart_read_reg(LSR) & LSR_RX_READY ));
	char charin = uart_read_reg(RHR);
	// 再内部调用uart_putc(charin)实现回显的consol
	uart_putc(charin);

	return charin;	
}


void *uart_gets(char *buf, uint8_t length) 
{

	for (uint8_t counter = 0; counter < length; counter++) {
		char charin = uart_getc();
		if (charin == '\b')	*buf++ = '!'; 	
		else {
			*buf++ = charin;
			if (charin == '\n') break;
		}

	}

}
void uart_putstr(char *s)
{
	
	uart_putc('\n');	
	while (*s) {
		while ((uart_read_reg(LSR) & LSR_TX_IDLE) == 0  );
		uart_write_reg(THR, *s++);

	}	
}

void uart_puts(char *s)
{
	while (*s) {
		uart_putc(*s++);
	}
}

