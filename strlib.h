/*
 * strlib.h
 *
 *  Created on: 2021/11/25
 *      Author: Yuji Sugiyama
 */

#ifndef STRLIB_H_
#define STRLIB_H_

typedef unsigned char	BYTE;
typedef	unsigned short	WORD;

extern int	wtou(WORD, char*);
extern int	wtos(int, char*);
extern int	wtox(WORD, char*);
extern int	puts(const char *);

#endif /* STRLIB_H_ */
