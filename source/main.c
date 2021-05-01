/*	Author: ejack023
 *	Lab Section: 023
 *	Assignment: Lab #6  Exercise #1
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://www.youtube.com/watch?v=vAwiYpvYqsU&ab_channel=EthanJackson
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
	TCCR1B=0x0B;
	
	OCR1A = 125;
	
	TIMSK1 = 0x02;
	
	TCNT1 = 0;
	
	_avr_timer_cntcurr = _avr_timer_M;
	
	SREG |= 0x80;
}
void TimerOff() {
	TCCR1B = 0x00;
}
void TimerISR() {
	TimerFlag=1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;	
}

enum SM_STATES {SM_start, SM_first, SM_second, SM_third} SM_STATE;

void Tick_Fct() {
	switch(SM_STATE) {
		case SM_start:
			SM_STATE = SM_first;
			break;
		case SM_first:
			SM_STATE = SM_second;
			break;
		case SM_second:
			SM_STATE = SM_third;
			break;
		case SM_third:
			SM_STATE = SM_first;
			break;
		default:
			SM_STATE = SM_first;
			break; 
	}
	switch(SM_STATE) {
		case SM_start:
			break;
		case SM_first:
			PORTB = 0x01;
			break;
		case SM_second:
			PORTB = 0x02;
			break;
		case SM_third:
			PORTB = 0x04;
			break;
		default:
			PORTB = 0x01;
			break;
	}
}

int main(void)
{
	DDRB = 0xFF; PORTB = 0x00;
	TimerSet(1000);
	TimerOn();
	SM_STATE = SM_start;
	while(1) {
		Tick_Fct();
		while(!TimerFlag);
		TimerFlag = 0;
	}
}

