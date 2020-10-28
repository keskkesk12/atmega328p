#define __AVR_ATmega328P__
#define F_CPU 16000000L

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//Initialize functions
void setupADC();
void startConversion();
void setupPWM();

uint8_t duty_cycle;

int main(){

    sei();

    setupADC();
    setupPWM();

    while (1){
        
    }
    
    return 0;
}

void setupPWM(){
    //Setting PORTD6 as output for the PWM
    DDRD = (1 << PORTD6);
    /* 
        WGM01:WGM00; Sets the timer control register to perform 'Fast PWM'
        COM0A1; Sets up behavior of OC0A
     */
    TCCR0A = (1 << COM0A1) | (1 << WGM01) | (1 << WGM00);
    /* 
        Setting the clock source to the I/O clock with 64 prescaling
        This enables the clock since 0-0-0 in CS02:CS00 is "clock disabled"
     */
    TCCR0B = (1 << CS01) | (1 << CS00);
    TIMSK0 = (1 << TOIE0);
}

void setupADC(){
    /* 
        REFS0; Setup reference voltage
        MUX2:MUX0; Select pin with multiplexer
     */
    ADMUX = (1 << REFS0) | (1 << MUX2) | (1 << MUX0) | (1 << ADLAR);
    
    /* 
        ADEN; Enable ADC
        ADIE; ADC Interrupt Enable
        ADPS; ADC Clock prescalar. 32 prescalar is selected
    */
    ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);

    /* 
        Disable digital inputs
     */
    DIDR0 = (1 << ADC5D);
    startConversion();
}

void startConversion(){
    /* 
        Starts ADC conversion.
        When the conversion is done an interrupt is made
     */
    ADCSRA |= (1 << ADSC);
}

ISR(TIMER0_OVF_vect){
    OCR0A = duty_cycle;
}

ISR(ADC_vect){
    //Update Output Compare Register
    duty_cycle = ADCH;

    //Start new conversion
    startConversion();
}


 