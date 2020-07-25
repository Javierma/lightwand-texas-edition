/*
 * 1wire.h
 *
 *  Created on: 16/10/2016
 *      Author: Javier
 */



#ifndef 1WIRE_H_
#define 1WIRE_H_



#endif /* 1WIRE_H_ */

#define NUM_LEDS 144

void send_bit(char bit);
void init_1wire_pin(void);
void set_brightness(char brightness);
void set_delay(unsigned long delay);
char get_brightness(void);
unsigned long get_delay(void);
void clear_ledarray(void);
