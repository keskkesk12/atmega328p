#define __AVR_ATmega328P__
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>

//General codes
#define START           0x08
#define REPEAT_START    0x10
#define ARB_LOST        0x38

//Master transmit
#define MT_SLA_ACK      0x18
#define MT_SLA_NACK     0x20
#define MT_DATA_ACK     0x28
#define MT_DATA_NACK    0x30

//Master receive
#define MR_SLA_ACK      0x40
#define MR_SLA_NACK     0x48
#define MR_DATA_ACK     0x50
#define MR_DATA_NACK    0x58

//MPU defines
#define MPU_ADR         0x68
#define ACC_X           0x3B
#define ACC_Y           0x3D
#define ACC_Z           0x3F
#define GYR_X           0x43
#define GYR_Y           0x45
#define GYR_Z           0x47


void setupSerial(long baud, long cpu_freq);
void sendChar(char val);

void imu_sel_reg(unsigned char reg);
void twi_init();
void imu_write_byte(unsigned char reg, unsigned char data);
int16_t imu_read_16(unsigned char reg);
unsigned char imu_test();
void imu_init();
void sendArray(char array[]);

char buffer1[7];

int main(){
    //Initialize twi
    twi_init();
    imu_init();
    setupSerial(9600, F_CPU);

    DDRC |= 0x0F;
    DDRB |= 0x1F;
    
    while(1){
        int16_t x = imu_read_16(0x3D);
        int16_t y = imu_read_16(0x43);

        itoa(x, buffer1,10);

        sendArray(buffer1);
        sendChar(',');

        itoa(y, buffer1, 10);
        
        sendArray(buffer1);
        sendArray("\r\n");

        _delay_ms(10);
        //PORTB ^= (1 << PORTB1);
    }
    return 0;
}

void error(){
    PORTB |= (1 << PORTB0);
}

//Set SCL to 400.000
void twi_init(){
    //Set bit rate
    TWBR = 12;
}

void imu_init(){
    //Exit sleep mode
    imu_write_byte(0x6B, 0);
    
    //Config accelerometer
    //+-8g
    imu_write_byte(0x1C, 0x10);

    //Config gyroscope
    //+-500 deg/s
    imu_write_byte(0x1B, 0x08);
}

void imu_write_byte(unsigned char reg, unsigned char data){
    //Select register for writing
    imu_sel_reg(reg);

    //Write data
    TWDR = data;
    //Initialize transmission
    TWCR = (1 << TWINT) | (1 << TWEN);
    //Wait for bits to be sent
    while(!(TWCR & (1 << TWINT)));
    //Check status
    if((TWSR & 0xF8) != MT_DATA_ACK)
        error();

    //Stop transmission
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}

int16_t imu_read_16(unsigned char reg){
    //Select start register
    imu_sel_reg(reg);
    
    //Send repeated start bit
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);
    //Wait for bits to be sent
    while(!(TWCR & (1 << TWINT)));
    //Check status
    if((TWSR & 0xF8) != REPEAT_START)
        error();
    
    //Send slave address and read
    TWDR = MPU_ADR << 1 | 1;
    //Initialize transmission
    TWCR = (1 << TWINT) | (1 << TWEN);
    //Wait for bits to be sent
    while(!(TWCR & (1 << TWINT)));
    //Check status
    if((TWSR & 0xF8) != MR_SLA_ACK)
        error();
    
    //Initialize read
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    //Wait for bits to be sent
    while(!(TWCR & (1 << TWINT)));
    //Read data
    int16_t upper = TWDR;
    //Check status
    if((TWSR & 0xF8) != MR_DATA_ACK)
        error();

    //Initialize read
    TWCR = (1 << TWINT) | (1 << TWEN);
    //Wait for bits to be sent
    while(!(TWCR & (1 << TWINT)));
    //Read data
    int16_t lower = TWDR;
    //Check status
    if((TWSR & 0xF8) != MR_DATA_ACK)
        error();

    //Send stop bit
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);

    //return upper << 8 | lower;
    return upper << 8 | lower;
}

void imu_sel_reg(unsigned char reg){
    //Send start bit
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);
    //Wait for bits to be sent
    while(!(TWCR & (1 << TWINT)));
    //Check status
    if((TWSR & 0xF8) != START)
        error();

    //Send slave address and write
    TWDR = MPU_ADR << 1 | 0;
    //Initialize transmission
    TWCR = (1 << TWINT) | (1 << TWEN);
    //Wait for bits to be sent
    while(!(TWCR & (1 << TWINT)));
    //Check status
    if((TWSR & 0xF8) != MT_SLA_ACK)
        error();

    //Select internal register in MPU6050
    TWDR = reg;
    //Initialize transmission
    TWCR = (1 << TWINT) | (1 << TWEN);
    //Wait for bits to be sent
    while(!(TWCR & (1 << TWINT)));
    //Check status
    if((TWSR & 0xF8) != MT_DATA_ACK)
        error();
}

void sendChar(char val){
    UDR0 = val;

    //Wait until data has been transmitted
    while (!(UCSR0A & (1<<UDRE0)));
}

void setupSerial(long baud, long cpu_freq){
    int ubr_calc = cpu_freq/16/baud-1;

    //Set up baud rate
    UBRR0H = (ubr_calc >> 8);
    UBRR0L = ubr_calc;

    //Enable USART transmitter
    //Enable USART receiver
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);

    //Set up 8-bit dataframe
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void sendArray(char array[]){
    for(int i = 0; i < strlen(array); i++){
        sendChar(array[i]);
    }
}
