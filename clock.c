/*
 * clock.c
 *
 *  Modified on: 2021/11/15
 *      Author: Yuji Sugiyama
 *
 */
#include <msp430.h>
#include "general.h"
#include "clock.h"

static uint16_t	wdt_clk = 0;
static uint32_t	wdt_msec = 0;		// upto 4,294,967,295msec(approx.49days)
#ifndef WDT_SMCLK
static uint16_t	wdt_frac = 0;
#endif

/*--------------------------------------------------------------------------------------------------
	Public routines
----------------------------------------------------------------------------------------------------*/
void ClockInit(int clk)
{
	ADC10AE0 = 0;
	CAPD = 0;

#ifdef ACLK
	P1DIR |= BIT0;
	P1SEL |= BIT0;
	P1SEL2 &= ~BIT0;				// P1.0 = ACLK/RED_LED
#endif

#ifdef SMCLK
	P1DIR |= BIT4;
	P1SEL |= BIT4;
	P1SEL2 &= ~BIT4;				// P1.4 = SMCLK(16MHz)
#endif

	BCSCTL2 = 0x00;					// MCLK = SMCLK = DCO /1 (16MHz)
	BCSCTL3 = 0x24;					// ACLK = VLO(11.6KHz as measured value, typical 12KHz) not LFXT

	switch(clk){
	case 1:
		BCSCTL1 = CALBC1_1MHZ;		// Set range
		DCOCTL = CALDCO_1MHZ;
		break;
	case 8:
		BCSCTL1 = CALBC1_8MHZ;		// Set range
		DCOCTL = CALDCO_8MHZ;
		break;
	case 12:
		BCSCTL1 = CALBC1_12MHZ;		// Set range
		DCOCTL = CALDCO_12MHZ;
		break;
	default:
		BCSCTL1 = CALBC1_16MHZ;		// Set range
		DCOCTL = CALDCO_16MHZ;
		break;
	}
}

void WatchDogInit(int clk)
{
	WDTCTL = WDTPW | WDTHOLD;								// Stop Watch-Dog timer

#ifdef WDT_SMCLK
	WDTCTL = WDTPW | WDTTMSEL | WDTCNTCL | WDT_MDLY_8;		// counter_overflow=8192(fix), overflow period is 8192/(SMCLK=clk) usec
	switch(clk){
	case 1:					// @SMCLK==1MHz then overflow occurs every 8192usec
		wdt_clk = 8192;
		break;
	case 8:					// @SMCLK==8MHz then overflow occurs every 8192/8=1024usec
		wdt_clk = 1024;		// 8192>>3;
		break;
	case 12:				// @SMCLK==12MHz then overflow occurs every 8192/12=682usec
		wdt_clk = 682;		// 8192/12;
		break;
	default:				// @SMCLK==16MHz then overflow occurs every 8192/16=512usec
		wdt_clk = 512;		// 8192>>4;	(default)
		break;
	}
#else	// ACLK(VLO)
	WDTCTL = WDTPW | WDTTMSEL | WDTCNTCL | WDT_ADLY_1_9;		// counter_overflow=64(fix), overflow period is 64/(ACLK=clk) msec
	wdt_clk = 5980;												// ACLK = 11600Hz approx.5.517msec / overflow, (5.500+0.017)x1000
	wdt_frac = 118;												// Adjusted to wdt_clk=5913, fract = 112
#endif
	IE1 |= WDTIE;
}

uint32_t GetMsec(void)
{
	return wdt_msec;
}

void DelayMsec(uint32_t msec)
{
	uint32_t	prev_time, curr_time;

	prev_time = curr_time = GetMsec();
	while(1){
		if(curr_time - prev_time >= msec)
			return;
		else
			curr_time = GetMsec();
	}
}

#if 0
void dummyLoop(int n)
{
	int	i, j;

	for(i=0; i<n; i++)
		for(j=0; j<1000; j++);
}
#endif

/*--------------------------------------------------------------------------------------------------
	Interrupt routines
----------------------------------------------------------------------------------------------------*/
#pragma vector = WDT_VECTOR
__interrupt void watchdogIsr(void)
{
	static uint16_t	wdt_acum = 0;

#ifdef WDT_SMCLK
	wdt_acum += wdt_clk;
	if(wdt_acum >= MS_SMCLK){				// Should be 1000 but some error are there
		wdt_acum = wdt_acum - MS_SMCLK;
		wdt_msec++;
	}
#else
	wdt_msec += 6;							// past 6msec
	wdt_acum += wdt_frac;
	if(wdt_acum >= wdt_clk){
		wdt_msec += 6;						// accumulated fraction > 6000
		wdt_acum = wdt_acum - wdt_clk;
	}
#endif

	// __bic_SR_register_on_exit(LPM1_bits);
#ifdef WDT_SMCLK
	LPM1_EXIT;
#else
	LPM3_EXIT;
#endif
}
