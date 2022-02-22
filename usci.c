/*
 * usci.c
 *
 *  Created on: 2021/11/12
 *      Author: Yuji Sugiyama
 */
#include <msp430.h>
#include "general.h"
#include "clock.h"
#include "usci.h"

static struct ringRXBuff	rxBuff = { { 0 }, 0, 0, 0 };
static struct ringTXBuff	txBuff = { { 0 }, 0, 0, 0 };

/*--------------------------------------------------------------------------------------------------
	Local routines
----------------------------------------------------------------------------------------------------*/
static void init(void)
{
	unsigned int	mod;
	unsigned long	bps_clk = (SMCLK_Hz<<4)/BAUDRATE;

	// RxD
	P1SEL |= BIT1;
	P1SEL2 |= BIT1;
	P1DIR &= ~BIT1;
	//TxD
	P1SEL |= BIT2;
	P1SEL2 |= BIT2;
	P1DIR |= BIT2;

	UCA0CTL1 = UCSWRST;
	UCA0CTL1 |= UCSSEL_2;					// SMCLK
	UCA0CTL0 = 0;
	UCA0ABCTL = 0;

	mod = ((bps_clk&0xf8)+0x08)&0xf0;		// UCBRFx (bit 4-7)
	bps_clk>>=8;
	UCA0BR0 = bps_clk;
	UCA0BR1 = bps_clk>>8;
	UCA0MCTL = (uint8_t)UCOS16 | mod;
	UCA0CTL0 = (UCA0CTL0 & ~(UCPEN|UCPAR|UC7BIT|UCSPB)) | SERIAL_8N1;
	UCA0CTL1 &= ~UCSWRST;
	UC0IE |= UCA0RXIE;
}

static uint8_t available(void)
{
	return rxBuff.count;
}

static uint8_t read(void)
{
	struct ringRXBuff	*p_rx = &rxBuff;
	uint8_t	c;

	if (p_rx->count == 0){					// No Rx data
		return 0;
	} else{
		UC0IE &= ~UCA0RXIE;
		c = p_rx->buff[p_rx->tail];
		incRXBuff(p_rx, tail);
		p_rx->count--;
		UC0IE |= UCA0RXIE;
		return c;
	}
}

static uint8_t write(uint8_t c)
{
	struct ringTXBuff	*p_tx = &txBuff;

	if(p_tx->count == TX_BUFF_SIZE){		// Tx buffer full
		return 0;
	} else{
		UC0IE &= ~UCA0TXIE;
		p_tx->buff[p_tx->head] = c;
		incTXBuff(p_tx, head);
		p_tx->count++;
		UC0IE |= UCA0TXIE;
		return 1;
	}
}

/*--------------------------------------------------------------------------------------------------
	Public routines
----------------------------------------------------------------------------------------------------*/
void USCI_uart_Init(void)
{
	init();
}

int USCI_uart_Available(void)
{
	return (int)available();
}

int USCI_uart_Read(void)
{
	return (int)read();
}

int USCI_uart_Write(uint8_t c)
{
	return (int)write(c);
}

/*--------------------------------------------------------------------------------------------------
	Interrupt routines
----------------------------------------------------------------------------------------------------*/
//#define USE_I2C

#ifdef USE_I2C
extern inline void I2CStatusIsr(void);
extern inline void I2CRxTxIsr(void);
#endif

#pragma vector = USCIAB0RX_VECTOR
__interrupt void USCIAB0RX_ISR(void)
{
	if(IFG2 & UCA0RXIFG){
		struct ringRXBuff	*p_rx = &rxBuff;
		uint8_t	c = UCA0RXBUF;

		if (p_rx->count != RX_BUFF_SIZE){		// Rx buffer not full
			p_rx->buff[p_rx->head] = c;
			incRXBuff(p_rx, head);
			p_rx->count++;
		}
	}

#ifdef USE_I2C
	I2CStatusIsr();								// if i2c to be used, call for status interrupt
#endif

#ifdef WDT_SMCLK
	LPM1_EXIT;
#else
	LPM3_EXIT;
#endif
}

#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_ISR(void)
{
	if((UC0IE & UCA0TXIE) && (IFG2 & UCA0TXIFG)){
		struct ringTXBuff	*p_tx = &txBuff;
		uint8_t	c;

		if (p_tx->count == 0){
			UC0IE &= ~UCA0TXIE;					// No Tx data
		} else{
			c = p_tx->buff[p_tx->tail];
			incTXBuff(p_tx, tail);
			p_tx->count--;
			UCA0TXBUF = c;
		}
	}

#ifdef USE_I2C
	I2CRxTxIsr();								// if i2c to be used, call for RxTx interrupt
#endif

#ifdef WDT_SMCLK
	LPM1_EXIT;
#else
	LPM3_EXIT;
#endif
}
