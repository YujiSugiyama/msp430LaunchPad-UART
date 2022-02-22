/*
 * clock.h
 *
 *  Modified on: 2021/11/15
 *      Author: Yuji Sugiyama
 */
#ifndef CLOCK_H_
#define CLOCK_H_

#define SMCLK_Hz		16000000UL

#define ACLK
#define SMCLK

#define WDT_SMCLK
#define	MS_SMCLK		1000			// for 1msec @SMCLK

// Public routines
extern void ClockInit(int clk);
extern void WatchDogInit(int clk);
extern uint32_t GetMsec(void);
extern void DelayMsec(uint32_t);

#endif /* CLOCK_H_ */
