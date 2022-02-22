/*
 * timerA.c
 *
 *  Created on: 2022/1/25
 *      Author: Yuji Sugiyama
 */
#include <msp430.h>
#include "general.h"
#include "clock.h"
#include "timerA.h"

/*--------------------------------------------------------------------------------------------------
	Public routines
----------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------
	TimerA0 for S/W UART routines!!!
		Case P1 : TxD=P1.1(TA), RxD=P1.2(CCR1) <- Now in use
		Case P2 : TxD=P2.3(TA), RxD=P2.4(CCR2)
----------------------------------------------------------------------------------------------------*/
static volatile uint16_t	TxData;
static volatile int8_t		TxCount;
static volatile uint16_t	RxData;
static volatile int8_t		RxCount;

static struct ringTABuff	rxBuff = { { 0 }, 0, 0, 0 };

void TMRA_uart_Init(void)
{
	TACTL = ID_1 + TASSEL_2 + MC_2;			// Dividing 1, SMCLK(16MHz), CountUpto 0xFFFF

	// P1.1/P2.3 CCR0 for TxD
#if TimerA==0
	P1DIR |= BIT1;							// OUT
	P1SEL &= ~BIT1;							// P1.1 I/O
	P1SEL2 &= ~BIT1;						// P1.1 I/O
#else
	P2DIR |= BIT3;							// OUT
	P2SEL &= ~BIT3;							// P2.3 I/O
	P2SEL2 &= ~BIT3;						// P2.3 I/O
#endif
	TACCTLx = OUT;							// Set TXD Idle state as High

	TxData = 0;
	TxCount = 10;

	// P1.2/P2.4 CCR1/2 for RxD
#if TimerA==0
	P1DIR &= ~BIT2;							// IN
	P1SEL |= BIT2;							// P1.2 CCI2A
	P1SEL2 &= ~BIT2;						// P1.2 CCI2A
#else
	P2DIR &= ~BIT4;							// IN
	P2SEL |= BIT4;							// P2.4 CCI2A
	P2SEL2 &= ~BIT4;						// P2.4 CCI2A
#endif
	TACCTLy = CM_2 + CAP + CCIE;			// Falling edge interrupt, Capture mode, Enable interrupts

	RxData = 0;
	RxCount = 0;
	rxBuff.head = rxBuff.tail = rxBuff.count = 0;
}

uint8_t TMRA_uart_Avilable(void)
{
	return rxBuff.count;
}

void TMRA_uart_Write(uint8_t c)
{
	while(TACCTLx & CCIE);

	register unsigned val = c | 0x100;		// Add stop bit '1'
	val <<= 1;								// Add the start bit '0'
	TxData = val;							// transmit byte
	TxCount = 10;

	TACCRx = TAR;							// Sync with current TimerA clock
	TACCRx += TICKS_PER_BIT_DIV2;			// Setup the next timer tick
	TACCTLx = OUTMOD_0 + CCIE;				// Set TX HIGH and enable interrupts
}

uint8_t TMRA_uart_Read(void)
{
	struct ringTABuff	*p_rx = &rxBuff;
	uint8_t	c;

	if (p_rx->count == 0){					// No Rx data
		return 0;
	} else{
		c = p_rx->buff[p_rx->tail];
		incTABuff(p_rx, tail);
		p_rx->count--;
		return c;
	}
}

/*--------------------------------------------------------------------------------------------------
	Interrupt routines for TimerUART
----------------------------------------------------------------------------------------------------*/
#if TimerA==0
#pragma vector=TIMER0_A0_VECTOR				// Tx interrupt
#else
#pragma vector=TIMER1_A0_VECTOR				// Tx interrupt
#endif
__interrupt void timerTxIsr(void)			// TimerA CCR0 compare match: Just past TICKS_PER_BIT
{
	TACCRx += TICKS_PER_BIT_DIV2;			// Setup the next timer tick

#if TimerA==0
	if(TxData & 0x01)	P1OUT |= BIT1;		// look at LSB if 1 then set OUT
	else				P1OUT &= ~BIT1;
#else
	if(TxData & 0x01)	P2OUT |= BIT3;		// look at LSB if 1 then set OUT
	else				P2OUT &= ~BIT3;
#endif
	TxData >>= 1;

	if(--TxCount <= 0)
		TACCTLx &= ~CCIE;					// disable interrupt, indicates we are done

#ifdef WDT_SMCLK
	LPM1_EXIT;
#else
	LPM3_EXIT;
#endif
}

#if TimerA==0
#pragma vector=TIMER0_A1_VECTOR					// Rx Interrupt
#else
#pragma vector=TIMER1_A1_VECTOR					// Rx Interrupt
#endif
__interrupt void timerRxIsr(void)				// TimerA CCR1/2 compare match
{
	volatile uint16_t	resetTAIV = TAIV;		// Just to clear CCI1FG
	register uint16_t	regCCTL = TACCTLy;		// Get CCTL1/2 value

	if(regCCTL & CAP){       					// Now it's got a start bit
	    TACCRy += TICKS_PER_BIT_DIV3;			// Adjust sample time, so next sample is in the middle of the bit width
	    RxCount = 0;							// Count Start
	    RxData = 0;								// Reset RxData
	    TACCTLy &= ~CAP;						// Switch from capture mode to compare mode
	} else{
		TACCRy += TICKS_PER_BIT_DIV2;			// Adjust sample time, so next sample is in the middle of the bit width
#if TimerA==0
		if(P1IN & BIT2)
			RxData |= (1 << RxCount);			// Set bit
#else
		if(P2IN & BIT4)
			RxData |= (1 << RxCount);			// Set bit
#endif
		RxCount++;
		if(RxCount >= 8){
			TACCTLy |= CAP;						// Switch back to capture mode and wait for next start bit

			struct ringTABuff	*p_rx = &rxBuff;
			if (p_rx->count != TA_RX_BUFF_SIZE){	// Rx buffer not full
				p_rx->buff[p_rx->head] = RxData;
				incTABuff(p_rx, head);
				p_rx->count++;
			}
		}
	}

#ifdef WDT_SMCLK
	LPM1_EXIT;
#else
	LPM3_EXIT;
#endif
}

