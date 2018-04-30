/*
 * uart.c
 *
 *  Created on: 29/10/2017
 *      Author: Javier
 */
#include "uart.h"

unsigned char UART_InChar(void){
  while((UART1_FR_R&UART_FR_RXFE) != 0);
  return((unsigned char)(UART1_DR_R&0xFF));
}

void UART_OutChar(unsigned char data){
  while((UART1_FR_R&UART_FR_TXFF) != 0);
  printf("\nData sent: %c",data);
  UART1_DR_R=data;
  //int l=0;
}

void initialise_uart()
{
	volatile unsigned long delay;
	SYSCTL_RCGCUART_R |=SYSCTL_RCGCUART_R1; //Enable clock for the UART module
	SYSCTL_RCGC2_R |= 0x00000004;   //  activate clock for Port C
	while((SYSCTL_PRGPIO_R2&0x0004) == 0){};// ready?
	UART1_CTL_R &= ~UART_CTL_UARTEN;
	UART1_IBRD_R=104;//104
	UART1_FBRD_R=11;//11
	UART1_LCRH_R=0x70;
	UART1_CC_R=0x00;
	UART1_CTL_R|= UART_CTL_RXE | UART_CTL_TXE | UART_CTL_EOT | UART_CTL_UARTEN;

	GPIO_PORTC_AFSEL_R |= 0x30;           // enable alt funct on PC4 and 5
	GPIO_PORTC_DEN_R |= 0x30;   // enable digital I/O on PC4 and 5
    // configure PC4 and 5 as UART
	GPIO_PORTC_PCTL_R = (GPIO_PORTC_PCTL_R&0xFF00FFFF)+0x00220000;
	GPIO_PORTC_AMSEL_R&=~0x30;
}
