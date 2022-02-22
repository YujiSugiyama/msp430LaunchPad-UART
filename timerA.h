/*
 * timerA.h
 *
 *  Created on: 2022/1/25
 *      Author: Yuji Sugiyama
 */
#ifndef TIMERA_H_
#define TIMERA_H_

#define	TimerA	1			// 0:TimerA0, 1:TimerA1
#undef TACTL
#undef TACCR0
#undef TACCR2
#undef TACCTL0
#undef TACCTL2
#undef TAR
#undef TAIV
#if TimerA==0
	#define TACTL	TA0CTL
	#define TACCRx	TA0CCR0
	#define TACCRy	TA0CCR1			// Note) Not TA0CCR2 but TA0CCR1
	#define TACCTLx	TA0CCTL0
	#define TACCTLy	TA0CCTL1		// Note) Not TA0CCTL2 but TA0CCTL1
	#define TAR		TA0R
	#define TAIV	TA0IV
#else
	#define	TACTL	TA1CTL
	#define TACCRx	TA1CCR0
	#define TACCRy	TA1CCR2			// Note) Not TA0CCR1 but TA0CCR2
	#define TACCTLx	TA1CCTL0
	#define TACCTLy	TA1CCTL2		// Note) Not TA0CCTL1 but TA0CCTL2
	#define	TAR		TA1R
	#define	TAIV	TA1IV
#endif

// TimerA
#define TA_BAUDRATE			9600UL
#define TICKS_PER_BIT		(SMCLK_Hz / TA_BAUDRATE)						// 1600000/9600Hz
#define TICKS_PER_BIT_DIV2	(TICKS_PER_BIT >> 1)							// 1600000/9600/2Hz
#define TICKS_PER_BIT_DIV3	((TICKS_PER_BIT >> 1) + (TICKS_PER_BIT >> 2))	// (1600000/9600)*(1.5)Hz

// Public routines
void TMRA_uart_Init(void);
uint8_t TMRA_uart_Avilable(void);
void TMRA_uart_Write(uint8_t);
uint8_t TMRA_uart_Read(void);

#if UART == 0
#define putchar(c)	TMRA_uart_Write(c)
#define getchar()	TMRA_uart_Read()
#define received()	TMRA_uart_Avilable()
#endif

struct ringTABuff
{
#define TA_RX_BUFF_SIZE	16
	volatile uint8_t	buff[TA_RX_BUFF_SIZE];
	volatile uint8_t	head;
	volatile uint8_t	tail;
	volatile uint8_t	count;
};

#define incTABuff(p, a)	\
	if((p->a + 1) == TA_RX_BUFF_SIZE)	p->a = 0;	\
	else	p->a++

#endif /* TIMERA_H_ */
