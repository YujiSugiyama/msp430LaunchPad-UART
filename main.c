/*
 * IntegratedPad STDmain.c
 *  Standard template project
 *  Created on: 2022/1/25
 *      Author: Yuji Sugiyama
 */
#include <msp430.h> 
#include "strlib.h"
#include "general.h"
#include "clock.h"
#if UART == 0
#include "timerA.h"
#else
#include "usci.h"
#endif
//#include "i2c_master.h"

/*--------------------------------------------------------------------------------------------------
	Local routines
----------------------------------------------------------------------------------------------------*/
/*
 * Configure Push Button: Refer to UserGuideForMSP430.P328~
 */
static void initButtons(void)
{
	P1SEL &= ~BIT3;			// Button IO mode
	P1SEL2 &= ~BIT3;		//
	P1DIR &= ~BIT3;			// In
	P1REN |= BIT3;			// Pull up/down
	P1OUT |= BIT3;			// Pull up
	P1IES |= BIT3;			// H->L Edge Interrupt
	P1IFG &= ~BIT3;			// Interrupt Flag Clear
	P1IE |= BIT3;			// Interrupt Enable
}

/*
 * Configure LEDs: Refer to MSP430G2553.P43~
 */
static void initLEDs(void)
{
	// P1
	P1SEL &= ~BIT0;			// RED_LED IO mode
	P1SEL2 &= ~BIT0;		// IO mode
	P1DIR |= BIT0;			// Out
	P1OUT &= ~BIT0;			// LEDs Off
}

/*--------------------------------------------------------------------------------------------------
	Public routines
----------------------------------------------------------------------------------------------------*/
static uint8_t	apl_run = 1;

/*
 * main()
 */
 void main(void)
{
	uint32_t	prev_time, curr_time;

	WDTCTL = WDTPW | WDTHOLD;		// Stop Watch-Dog timer
	P1DIR = 0xFF, P1OUT = 0x00;		// Port1 should be out and low
	P2DIR = 0xFF, P2OUT = 0x00;		// Port2 should be out and low
	P3DIR = 0xFF, P3OUT = 0x00;		// Port3 should be out and low (Port3 is not available)

	ClockInit(16);					// MCLK=SMCLK=16MHz(DCO), ACLK=approx.12KHz(VLO)
	WatchDogInit(16);				// SMCLK=16MHz
#if UART == 1
	USCI_uart_Init();				// USCI UART
#else
	TMRA_uart_Init();				// TimerA UART
#endif
//	I2CInit();						// Connect out in case of SCL/SDA pin test

	initButtons();
	initLEDs();

	__enable_interrupt();			// Enable interrupts.
	prev_time = GetMsec();

	puts("Start UART example\n");

	uint8_t	interval = 0;
	uint8_t	pause = 0;
	const int16_t	speed = 100;
	while(1){
		if(received()){
			char c = getchar();
			putchar(c);				// Echo the character
		}

//--- Round robin application: ---
		curr_time = GetMsec();
		if(curr_time - prev_time >= speed){
			prev_time = curr_time;
			if(apl_run){
				// Do something once per 100msec
				puts("Running every 100ms\n");
				pause = 0;
			} else if(!pause){
				puts("Pause!!!\n");
				pause = 1;
			}
			if(++interval >= 10){
				P1OUT ^= BIT0;		// RED_LED blink per 1sec
				interval = 0;
			}
		}
//--- End of Round robin application ---

#ifdef WDT_SMCLK
		LPM1;			// LPM1 with interrupts enabled
#else
		LPM3;			// LPM3 with interrupts enabled
#endif
	}
}

/*--------------------------------------------------------------------------------------------------
	Interrupt routines
	Chatter is not considered
----------------------------------------------------------------------------------------------------*/
#pragma vector = PORT1_VECTOR
__interrupt void port1Isr(void)
{
	P1IFG &= ~BIT3;			// clear flag
	apl_run ^= 1;			// disable application

#ifdef WDT_SMCLK
	LPM1_EXIT;
#else
	LPM3_EXIT;
#endif
}

