/*
 * strlib.c
 *
 *  Created on: 2021/11/25
 *      Author: Yuji Sugiyama
 */
#include <msp430.h>
#include "general.h"
#include "strlib.h"
#include "clock.h"
#include "timerA.h"
#if UART == 1
#include "usci.h"
#else
#include "timerA.h"
#endif

/*--------------------------------------------------------------------------------------------------
	Local routines
----------------------------------------------------------------------------------------------------*/
static int getuwfig(WORD d)
{
	if(d <= 9)		return(1);
	if(d <= 99)		return(2);
	if(d <= 999)	return(3);
	if(d <= 9999)	return(4);
	return(5);
}

static int getxwfig(WORD d)
{
	if(d <= 0x000f) return(1);
	if(d <= 0x00ff) return(2);
	if(d <= 0x0fff) return(3);
	return(4);
}

static int num2code(BYTE d)
{
	if(d<=0x09){
		return((char)(d + 0x30));					// '0' - '9'
	} else if(d>= 0x0A && d<=0x0F){
		return((char)(d + 0x37));					// 'A' - 'F'
	} else{
		return((char)-1);
	}
}

/*--------------------------------------------------------------------------------------------------
	Public routines
----------------------------------------------------------------------------------------------------*/
int wtou(WORD d, char *str)
{
	uint16_t	len, i;

	len = getuwfig(d);
	str[len] = 0;
	for(i=len; i!=0; i--){
		str[i-1] = d % 10 + 0x30;
		d /= 10;
	}
	return(len);
}

int wtod(int d, char *str)
{
	if(d >= 0)
		return(wtou((WORD)d, str));
	else{
		*str++ = '-';
		return(wtou((WORD)-d, str) + 1);
	}
}

int wtox(WORD d, char *str)
{
	uint16_t	len, i;

	len = getxwfig(d);
	str[len] = 0;
	for(i=len; i!=0; i--){
		str[i-1] = num2code(d & 0x000f);
		d >>= 4;
	}
	return(len);
}

int puts(const char *str)
{
	int		i = 0;

#if 0						// For H/W UART
	while(*str){
		if(!putchar(*str)){
#ifdef WDT_SMCLK
			LPM1;			// LPM1 with interrupts enabled
#else
			LPM3;			// LPM3 with interrupts enabled
#endif
		} else{
			str++;
			i++;
		}
	}
#else
	while(*str){			// For S/W UART
		putchar((uint8_t)*str);
		str++;
		i++;
	}
#endif
	return i;
}

