/*
 * 1wire.c
 *
 *  Created on: 16/10/2016
 *      Author: Javier
 */

#include "inc/tm4c123gh6pm.h"
#include "1wire.h"
#include <stdint.h>

char lightwand_brightness=5;
unsigned long lightwand_delay;

void init_1wire_pin()
{
	//SysTick_Init();
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x02;   		// activate clock for Port B
	delay = SYSCTL_RCGC2_R;         		// allow time for clock to stabilize
	GPIO_PORTB_DIR_R |= 0x80;             // make PB7 in to avoid iluminating the leds
	GPIO_PORTB_AFSEL_R &= ~0x80;           // disable alt funct on PB7
	GPIO_PORTB_DEN_R |= 0x80;             // enable digital I/O on PB7
	GPIO_PORTB_AMSEL_R &= ~0x80;          // disable analog functionality on PB7
	GPIO_PORTB_DATA_R|=0x80;
	GPIO_PORTB_DATA_R &= ~0x80;
}

void send_bit(char bit)
{
	if(bit==1)
	{
		GPIO_PORTB_DATA_R|=0x80;
		int i=0;
		i=0;
		GPIO_PORTB_DATA_R&=~0x80;
	}
	else
	{
		GPIO_PORTB_DATA_R^=0x80;
		GPIO_PORTB_DATA_R&=~0x80;
	}
}

void set_brightness(char brightness)
{
	lightwand_brightness=brightness;
}

char get_brightness(void)
{
	return lightwand_brightness;
}

void set_delay(unsigned long delay)
{
	lightwand_delay=delay;
}

unsigned long get_delay(void)
{
	return lightwand_delay;
}

void test_colors(void)
{
	char i,j,k;
	uint8_t pui8Colors[1][3]={{25,0,0}};
	for(i=0;i<NUM_LEDS;i++)
	{
		for(j=0;j<3;j++)
		{
			for(k=0;k<8;k++)
			{
				if(pui8Colors[0][j] & (0x80 >> k))
				{
					send_bit(1);
				}
				else
				{
					send_bit(0);
				}
			}
		}
	}
	//Reset
	GPIO_PORTB_DATA_R &= ~0x80;
	SysTick_Wait50ms(100);
	pui8Colors[0][0]=0;
	pui8Colors[0][1]=25;
	pui8Colors[0][2]=0;
	for(i=0;i<NUM_LEDS;i++)
	{
		for(j=0;j<3;j++)
		{
			for(k=0;k<8;k++)
			{
				if(pui8Colors[0][j] & (0x80 >> k))
				{
					send_bit(1);
				}
				else
				{
					send_bit(0);
				}
			}
		}
	}
	GPIO_PORTB_DATA_R &= ~0x80;
	SysTick_Wait50ms(100);
	pui8Colors[0][0]=0;
	pui8Colors[0][1]=0;
	pui8Colors[0][2]=25;
	for(i=0;i<NUM_LEDS;i++)
	{
		for(j=0;j<3;j++)
		{
			for(k=0;k<8;k++)
			{
				if(pui8Colors[0][j] & (0x80 >> k))
				{
					send_bit(1);
				}
				else
				{
					send_bit(0);
				}
			}
		}
	}
	GPIO_PORTB_DATA_R &= ~0x80;
	SysTick_Wait50ms(100);
	pui8Colors[0][0]=0;
	pui8Colors[0][1]=0;
	pui8Colors[0][2]=0;
	for(i=0;i<NUM_LEDS;i++)
	{
		for(j=0;j<3;j++)
		{
			for(k=0;k<8;k++)
			{
				if(pui8Colors[0][j] & (0x80 >> k))
				{
					send_bit(1);
				}
				else
				{
					send_bit(0);
				}
			}
		}
	}
	GPIO_PORTB_DATA_R &= ~0x80;
	SysTick_Wait50ms(100);
}

void clear_ledarray(void)
{
	char i,j,k;
	GPIO_PORTB_DATA_R &= ~0x80;
	SysTick_Wait(802);
	for(i=0;i<NUM_LEDS;i++)
	{
		for(j=0;j<3;j++)
		{
			for(k=0;k<8;k++)
			{
				send_bit(0);
			}
		}
	}
	GPIO_PORTB_DATA_R &= ~0x80;
}

