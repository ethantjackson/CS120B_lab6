/*	Author: ejack023
 *	Lab Section: 023
 *	Assignment: Lab #6  Exercise #2
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *	
 *	Demo Link: https://www.youtube.com/watch?v=EVVRlEpKLLI&ab_channel=EthanJackson
 *	        */

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

enum SM_STATES {SM_start, SM_first, SM_secondUp, SM_secondDown, SM_third, SM_pause} SM_STATE;
unsigned char prevInput = 0x00;
volatile unsigned char input = 0x00;

void Tick_Fct() {
	input = ~PINA & 0x01;
	switch(SM_STATE) {
		case SM_start:
			SM_STATE = SM_first;
			break;
		case SM_first:
			if (input && !prevInput) SM_STATE = SM_pause;
			else SM_STATE = SM_secondUp;
			break;
		case SM_secondUp:
			if (input && !prevInput) SM_STATE = SM_pause;
			else SM_STATE = SM_third;
			break;
		case SM_secondDown:
			if (input && !prevInput) SM_STATE = SM_pause;
			else SM_STATE = SM_first;
			break;
		case SM_third:
			if (input && !prevInput) SM_STATE = SM_pause;
			else SM_STATE = SM_secondDown;
			break;
		case SM_pause: 
			if (input && !prevInput) SM_STATE = SM_first;
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
		case SM_secondUp:
		case SM_secondDown:
			PORTB = 0x02;
			break;
		case SM_third:
			PORTB = 0x04;
			break;
		case SM_pause:
			 PORTB = PORTB;
			break;
		default:
			PORTB = 0x01;
			break;
	}
	prevInput = input;
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	TimerSet(300);
	TimerOn();
	SM_STATE = SM_start;
	while(1) {
		Tick_Fct();
		while(!TimerFlag);
		TimerFlag = 0;
	}
}

