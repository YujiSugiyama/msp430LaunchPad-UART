/*
 * usci.h
 *
 *  Created on: 2021/11/15
 *      Author: Yuji Sugiyama
 */
#ifndef USCI_H_
#define USCI_H_

#define BAUDRATE		9600UL		// 2400UL 9600UL 38400UL 115200UL(Upto 115200, because over-sampling is not supported)

#define SERIAL_7N1		(UC7BIT)
#define SERIAL_8N1		(0)
#define SERIAL_7N2		(UC7BIT                 | UCSPB)
#define SERIAL_8N2		(0                      | UCSPB)
#define SERIAL_7E1		(UC7BIT | UCPEN | UCPAR)
#define SERIAL_8E1		(0      | UCPEN | UCPAR)
#define SERIAL_7E2		(UC7BIT | UCPEN | UCPAR | UCSPB)
#define SERIAL_8E2		(0      | UCPEN | UCPAR | UCSPB)
#define SERIAL_7O1		(UC7BIT | UCPEN)
#define SERIAL_8O1		(0      | UCPEN)
#define SERIAL_7O2		(UC7BIT | UCPEN         | UCSPB)
#define SERIAL_8O2		(0      | UCPEN         | UCSPB)

// Public routines
extern void	USCI_uart_Init(void);
extern int	USCI_uart_Available(void);
extern int	USCI_uart_Read(void);
extern int	USCI_uart_Write(uint8_t);

#if UART == 1
#define putchar(c)	USCI_uart_Write(c)
#define getchar()	USCI_uart_Read()
#define received()	USCI_uart_Available()
#endif

struct ringRXBuff
{
#define RX_BUFF_SIZE	32
	volatile uint8_t	buff[RX_BUFF_SIZE];
	volatile uint8_t	head;
	volatile uint8_t	tail;
	volatile uint8_t	count;
};

#define incRXBuff(p, a)	\
	if((p->a + 1) == RX_BUFF_SIZE)	p->a = 0;	\
	else	p->a++

struct ringTXBuff
{
#define TX_BUFF_SIZE	16
	volatile uint8_t	buff[TX_BUFF_SIZE];
	volatile uint8_t	head;
	volatile uint8_t	tail;
	volatile uint8_t	count;
};

#define incTXBuff(p, a)	\
	if((p->a + 1) == TX_BUFF_SIZE)	p->a = 0;	\
	else	p->a++

#endif /* USCI_H_ */
