#define __AVR_ATmega328P__
#define F_CPU 16000000L

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


int main(){
    SREG |= (1 << SREG_I);
    
    //Ensure that PORTD4 is an input
    DDRD &= ~(1 << PORTD4);
    //Enable pull-up resistor on PD4
    PORTD = (1 << PORTD4);

    //Compare value
    OCR0A = 4;

    //Set PORTB 0-5 as output
    DDRB |= 0x1F;
    
    //Set CTC mode
    TCCR0A = (1 << WGM01);
    
    //Set up timer 0's clock source
    TCCR0B = (1 << CS01) | (1 << CS02);

    //Enable on interrupt routine on OCR0A
    TIMSK0 = (1 << OCIE0A);

    while (1){
        //Counter value is shifted due to the fact that the LED's are on pin 1-4
        PORTB = (TCNT0 << 1);
    }
    
    return 0;
}

ISR(TIMER0_COMPA_vect){
    PORTD ^= (1 << PORTD0);
}

