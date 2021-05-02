/* 
 * File:   uart.h
 * Author: tbriggs
 *
 * Created on October 7, 2013, 12:21 PM
 */

#ifndef UART_H
#define	UART_H
#include <stdio.h>

#include "circularbuff.h"

#ifdef	__cplusplus
extern "C" {
#endif


#define TXBUFFSZ CIRC_BUFF_SIZE
#define RXBUFFSZ CIRC_BUFF_SIZE

#define BAUDRATE   115200
#define USEXONXOFF 1


#ifdef CUSTOM_FGETS
char *	fgets(char *ptr, int num, FILE *stream);
#else
char *	myfgets(char *ptr, int num, FILE *stream);
#endif

void uart_echo(int enable);
void uart_init( );
void uart_fast_puts(const char *str);

// enable CR to LF mapping
extern int eolfix;

// buffering of STDIN/STDOUT or not
extern int buffering;


#ifdef	__cplusplus
}
#endif

#endif	/* UART_H */

