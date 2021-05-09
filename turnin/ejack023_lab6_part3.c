/*	Author: ejack023
 *	Lab Section: 023
 *	Assignment: Lab #6  Exercise #3
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://www.youtube.com/watch?v=l4wRx0oOEqY&ab_channel=EthanJackson
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

enum SM_STATES {SM_start, SM_init, SM_wait, SM_inc, SM_dec, SM_reset} SM_STATE;
unsigned char prevInput = 0x00;
volatile unsigned char input = 0x00;
unsigned char count = 0;

void Tick_Fct() {
	input = ~PINA & 0x03;
	switch(SM_STATE) {
		case SM_start:
			SM_STATE = SM_init;
			break;
		case SM_init:
			SM_STATE = SM_wait;
			break;
		case SM_wait:
			if (input & 0x02 && input & 0x01) SM_STATE = SM_reset;
			else if (input & 0x01 && !prevInput) {SM_STATE = SM_inc; count = 0;}
			else if (input & 0x02 && !prevInput) {SM_STATE = SM_dec; count = 0;}
			break;
		case SM_inc:
			if (input & 0x02 && input & 0x01) SM_STATE = SM_reset;
			else if (!(input & 0x01)) SM_STATE = SM_wait;
			break;
		case SM_dec:
			if (input & 0x02 && input & 0x01) SM_STATE = SM_reset;
			else if (!(input & 0x02)) SM_STATE = SM_wait;
			break;
		case SM_reset:
			if (!input) SM_STATE = SM_wait;
			break;
		default:
			SM_STATE = SM_wait;
			break; 
	}
	switch(SM_STATE) {
		case SM_start:
			break;
		case SM_init:
			PORTB = 7;
			break;
		case SM_wait:
			break;
		case SM_inc:
			if (PORTB < 9 && !(count%10)) ++PORTB;
			++count;
			break;
		case SM_dec:
			if (PORTB > 0 && !(count%10)) --PORTB;
			++count;
			break;
		case SM_reset:
			PORTB = 0;
			break;
		default:
			break;
	}
	prevInput = input;
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	TimerSet(100);
	TimerOn();
	SM_STATE = SM_start;
	while(1) {
		Tick_Fct();
		while(!TimerFlag);
		TimerFlag = 0;
	}
