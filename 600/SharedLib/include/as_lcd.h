#ifndef		__ASLCD_H__
#define		__ASLCD_H__


#include "as_uart.h"

// onely for ASCII
int lcd_textout(const unsigned char* buffer);
//int lcd_uart_open(uart_t*);
unsigned char BCD_fromat(int decimal);

#endif
