#define __AVR_ATmega328P__
#define F_CPU 16000000L

#include <avr/io.h>
#include <util/delay.h>

void setupADC();
int readADC();
    
int main(){

    setupADC();

    //Set all pins on PORTD as output
    DDRD = 0xFF;

    while(1){
        PORTD = readADC();
    }

    return 0;
}

void setupADC(){
    //Setup voltage reference and multiplexer
    //ADC reads from ADC5
    //Set ADLAR
    ADMUX = (1 << REFS0) | (1 << MUX0) |(1 << MUX2) | (1 << ADLAR);

    //Set prescalar
    //Enable ADC
    ADCSRA = (1 << ADEN) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);
}

int readADC(){
    //Start conversion
    ADCSRA |= (1 << ADSC);

    while(!(ADCSRA & (1<<ADIF))); //Wait until it finishes
    ADCSRA |= (1<<ADIF); //Clear flag

    //For 10-bit: Use ADCW instead of ADCH << 8 | ADCL and ADLAR = 0
    //FOr 8-bit: Use ADCH and ADLAR = 1
    return ADCH;
}
