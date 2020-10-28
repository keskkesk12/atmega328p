#define __AVR_ATmega328P__
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <avr/interrupt.h>

using namespace std;

void setupSerial(long baud, long cpu_freq);
void sendChar(char val);
void sendArray(char array[]);
char readChar();
char readString();
void setupPWM();
int readInt();

volatile char temp_num[4];
volatile bool read_flag = 0;
volatile char global_char = '0';

int duty_cycle = 255;

int main(){
    //Enable interrupts
    SREG = (1 << SREG_I);

    //Set up serial and PWM
    setupSerial(9600, F_CPU);
    setupPWM();


    //Set pins on PORTC as output
    DDRC = 0x3F;

    //Set pins on PORTB as output
    DDRB = (1 << DDB1) | (1 << DDB2);


    //Set OC0A to output
    DDRD = (1 << DDD6);


    while(1){
        //If full number is available
        if(read_flag){
            duty_cycle = readInt();
            read_flag = 0;
        }
        
        //Set LED's
        PORTC = duty_cycle >> 2;
        PORTB = (duty_cycle << 1) & 0x6;

        //Set duty cycle
        OCR0A = duty_cycle;
    }

    return 0;
}

int readInt(){
    int temp_duty_cycle = 0;

    int index = 0;

    while(temp_num[index] != '.'){
        //Read and convert to integer
        int num = (int)temp_num[index] - '0';

        temp_duty_cycle *= 10;
        temp_duty_cycle += num;
        index++;
    }

    return temp_duty_cycle;
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

//Receive interrupt routine
ISR(USART_RX_vect){
    //Counter that chooses position in array
    static int counter = 0;

    //Read char from serial
    char temp_char = UDR0;

    //Set char in array
    temp_num[counter] = temp_char;

    //Check if "end" char has been sent
    if(temp_char == '.'){
        //Reset counter
        counter = 0;
        //Set global read flag
        read_flag = 1;
        return;
    }

    //Increment counter
    counter ++;
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

