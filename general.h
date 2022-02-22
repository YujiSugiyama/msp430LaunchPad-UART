/*
 * general.h
 *
 *  Modified on: 2022/1/25
 *      Author: Yuji Sugiyama
 */
#ifndef GENERAL_H_
#define GENERAL_H_

typedef	signed char		int8_t;
typedef	signed int		int16_t;
typedef signed long		int32_t;
typedef	unsigned char	uint8_t;
typedef	unsigned int	uint16_t;
typedef unsigned long	uint32_t;

#define	UART	0		// 0:TIMERA0_UART, 1:USCI_UART

/*
 * Pin assignment
P1.0: RED_LED(J5-1 short), ACLK(J5-1 open)
P1.1: USCI_Rx or TA0.0_Tx
P1.2: USCI_Tx or TA0.1_Rx
P1.3: BUTTON
P1.4: SMCLK
P1.5:
P1.6: USCI_SCL(J5-2 open) or GREEN_LED(J5-2 short)
P1.7: USCI_SDA
P2.0:
P2.1:
P2.2:
P2.3: TA1.0_Tx
P2.4: TA1.2_Rx
P2.5:
 */

#endif /* GENERAL_H_ */
