#define __AVR_ATmega328P__
#define F_CPU 16000000L

#include <avr/io.h>
#include <util/delay.h>

void setupPWM();

int main(){

    /* 
        Button a - ADD
        Button b - SUB

        Button a - PORTC4
        Button b - PORTC5
     */

    uint8_t duty_cycle = 20;
    uint8_t step = 1;

    //Set PORTC 4 and 5 as input with pull-up
    PORTC = (1 << PORTC4) | (1 << PORTC5);

    //Set OC0A and OC0B as output
    DDRD = (1 << PORTD6) | (1 << PORTD5);

    setupPWM();

    while(1){
        //Check if button a has been pressed and debounce
        if(!(PINC & (1 << PORTC4))){
            duty_cycle += step;
            
            OCR0A = duty_cycle;
            OCR0B = duty_cycle;
            
            _delay_ms(100);
        }

        //Check if button b has been pressed and debounce
        if(!(PINC & (1 << PORTC5))){
            duty_cycle -= step;

            OCR0A = duty_cycle;
            OCR0B = duty_cycle;
            
            _delay_ms(100);
        }
    }

    return 0;
}

//Sets up PWM on OC0A and OC0B
void setupPWM(){

    //Clear OC0A on compare match, set OC0A at BOTTOM,(non-inverting PWM mode)
    //Clear OC0B on compare match, set OC0B at BOTTOM,(non-inverting PWM mode)
    //Set "waveform generation mode" to fast PWM
    TCCR0A |= (1 << COM0A1) | (1 << COM0B1) | (1 << WGM00) | (1 << WGM01);

    //Enable clock with no prescaling
    TCCR0B |= (1 << CS00);
}

