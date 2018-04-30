/*
 * uart.h
 *
 *  Created on: 29/10/2017
 *      Author: Javier
 */

#ifndef UART_H_
#define UART_H_

#include "inc/tm4c123gh6pm.h"
#include <stdint.h>

unsigned char UART_InChar(void);
void UART_OutChar(unsigned char data);
void initialise_uart();

#endif /* UART_H_ */
