/* 
    This program performs OR, AND and XOR on two buttons and shows the result via LED's
 */

#define __AVR_ATmega328P__
#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>

int main(){
    DDRB = (1 << DDB1) | (1 << DDB3); //Set pin 1 on PORTB as output
    DDRD = (1 << DDD0); //Set pin 0 on PORTD as output
    PORTC = (1 << PORTC5) | (1 << PORTC4); //Enable pull-up resistors

    bool both;
    bool none;
    bool red;
    bool white;
    bool exor;

    while (1){
        // if PINC5 is not high
        red = !(PINC & 0x20);
        white = !(PINC & 0x10);
        none = !(red | white);
        both = red & white;
        exor = red ^ white;

        if(both) 
            PORTB |= (1 << PORTB1);
        else 
            PORTB &= 0x02;

        if(none)
            PORTD |= (1 << PORTD0);
        else 
            PORTD &= 00;
        
        if(exor)
            PORTB |= (1 << PORTB3);
        else
            PORTB &= 0x04;
    }
    
    return 0;
}

