#define __AVR_ATmega328P__
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>

uint32_t position = 0;

char buffer[11];

void timer_init();
void sendChar(char val);
void sendArray(char array[]);
void setupSerial(long baud, long cpu_freq);


int main(){
    //Initialize timer
    timer_init();
    setupSerial(9600, F_CPU);

    while(1){
        //Increase position
        position += TCNT0;
        //Reset timer
        TCNT0 = 0;

        //Send position
        ultoa(position,buffer,10);
        sendArray(buffer);
        sendArray("\r\n");
    }

    return 0;
}


void timer_init(){
    //Set up timer input
    TCCR0B = (1 << CS02) | (1 << CS01);
}




void sendChar(char val){
    UDR0 = val;

    //Wait until data has been transmitted
    while (!(UCSR0A & (1<<UDRE0)));
}

void sendArray(char array[]){
    for(int i = 0; i < strlen(array); i++){
        sendChar(array[i]);
    }
}

void setupSerial(long baud, long cpu_freq){
    int ubr_calc = cpu_freq/16/baud-1;

    //Set up baud rate
    UBRR0H = (ubr_calc >> 8);
    UBRR0L = ubr_calc;

    //Enable USART transmitter
    //Enable USART receiver
    UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);

    //Set up 8-bit dataframe
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}