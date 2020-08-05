/*
 * File:   newmain.c
 * Author: TMaxi
 *
 * Created on 29 July, 2020, 9:31 AM
 */
#include <string.h>
#include <stdio.h>
#include <xc.h>
#include "lcd.h"
#include "delays.h"

unsigned char outchar, temperature;
char message[3];	  // Defining a 20 char string
int key = 0;

// this is a 1 sec timer for servomotor
void Delay1sec(void); // delay
void Delay1sec(void)
{
	TMR0H=0X48;			//Starting count value
	TMR0L=0XE5;		
		
	INTCONbits.TMR0IF=0;		//Clear flag first
	T0CONbits.TMR0ON=1;		//Turn on Timer 0
	
	while(INTCONbits.TMR0IF==0);	//Wait for time is up when TMR0IF=1 
	T0CONbits.TMR0ON=0;		//Turn off Timer 0 to stop counting
    // this is 1 sec delay
}

void main(void)
{   
    lcd_init(); // Initialise LCD module
    
    //ADCON1 = 0x0F;		//make Port A digital
    
    /* Analog to Digital ADCON setup
    // Initialise analog to digital conversion setting
    ADCON0 = 0b00000001;    // bit5-2 0000 select channel 0 conversion 
							// bit1	  A/D conversion status bit
							//	      1- GO to starts the conversion
							//	      0 - DONE when A/D is completed
							// bit0   Set to 1 to power up A/D

	ADCON1 = 0b00001100;	// bit5   reference is VSS
							// bit4   reference is VDD
							// bit3-0 AN2 to AN0 Analog, the rest Digital

	ADCON2 = 0b00010110;	// bit7   : A/D Result Format. 0 Left, 1 Right justified
							// bit5-3 : 010 acquisition time = 4 TAD
							// bit2-0 : 110 conversion clock = Tosc / 16
    */ 

    // Port A and C are Inputs, B and D are Outputs
	TRISA=0b11111111;	
    TRISB=0b00000000;
    TRISC=0b11111111;
    TRISD=0b00000000;
    
    /*
    // testing for servo motor
    while(1) {
        // change the bit number to where the motor is	 
        PORTBbits.RB1 = 1;	// on motor 
        Delay1sec();
        PORTBbits.RB1 = 0; // off motor
   	}
	*/ 
    
   
   
     
    // testing for LCD
	while(1)
	{
        
        
        ADCON0 = 0b00000001;
        ADCON1 = 0b00001110;
        ADCON2 = 0b00111101;
        
        ADCON0bits.GO = 1;
        
    
        while (ADCON0bits.GO == 1) {
            
            sprintf(message, "%d", ADRESH);
            ADCON0bits.GO = 0;
            
        }
        
        
		lcd_write_cmd(0x80);			// Move cursor to line 1 position 1
		for (key = 0; key < 3; key++)
		{
			outchar = message[key];
			lcd_write_data(outchar); 	// write character data to LCD
		}
	}     
}	


// found this code online for your temperature sensor
 
/*

//LCD module connections
#define LCD_RS_PIN      PIN_D0
#define LCD_RW_PIN      PIN_D1
#define LCD_ENABLE_PIN  PIN_D2
#define LCD_DATA4       PIN_D3
#define LCD_DATA5       PIN_D4
#define LCD_DATA6       PIN_D5
#define LCD_DATA7       PIN_D6
//End LCD module connections
 
#include <16F877A.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP                       
#use delay(clock = 8MHz)
#include <lcd.c>
#define DS18B20_PIN PIN_B1                         // DS18B20 Data pin is connected to pin RB1
 
signed int16 raw_temp;
float temp;
 
int1 ds18b20_start(){
  output_low(DS18B20_PIN);                         // Send reset pulse to the DS18B20 sensor
  output_drive(DS18B20_PIN);                       // Configure DS18B20_PIN pin as output
  delay_us(500);                                   // Wait 500 us
  output_float(DS18B20_PIN);                       // Configure DS18B20_PIN pin as input
  delay_us(100);                                   //wait to read the DS18B20 sensor response
  if (!input(DS18B20_PIN)) {
    delay_us(400);                                 // Wait 400 us
    return TRUE;                                   // DS18B20 sensor is present
  }
  return FALSE;
}
 
void ds18b20_write_bit(int1 value){
  output_low(DS18B20_PIN);
  output_drive(DS18B20_PIN);                       // Configure DS18B20_PIN pin as output
  delay_us(2);                                     // Wait 2 us
  output_bit(DS18B20_PIN, value);
  delay_us(80);                                    // Wait 80 us
  output_float(DS18B20_PIN);                       // Configure DS18B20_PIN pin as input
  delay_us(2);                                     // Wait 2 us
}
 
void ds18b20_write_byte(int8 value){
  int8 i;
  for(i = 0; i < 8; i++)
    ds18b20_write_bit(bit_test(value, i));
}
 
int1 ds18b20_read_bit(void) {
  int1 value;
  output_low(DS18B20_PIN);
  output_drive(DS18B20_PIN);                       // Configure DS18B20_PIN pin as output
  delay_us(2);
  output_float(DS18B20_PIN);                       // Configure DS18B20_PIN pin as input
  delay_us(5);                                     // Wait 5 us
  value = input(DS18B20_PIN);
  delay_us(100);                                   // Wait 100 us
  return value;
}
 
int8 ds18b20_read_byte(void) {
  int8 i, value = 0;
  for(i = 0; i  < 8; i++)
    shift_right(&value, 1, ds18b20_read_bit());
  return value;
}
 
int1 ds18b20_read(int16 *raw_temp_value) {
  if (!ds18b20_start())                     // Send start pulse
    return FALSE;
  ds18b20_write_byte(0xCC);                 // Send skip ROM command
  ds18b20_write_byte(0x44);                 // Send start conversion command
  while(ds18b20_read_byte() == 0);          // Wait for conversion complete
  if (!ds18b20_start())                     // Send start pulse
    return FALSE;                           // Return 0 if error
  ds18b20_write_byte(0xCC);                 // Send skip ROM command
  ds18b20_write_byte(0xBE);                 // Send read command
  *raw_temp_value = ds18b20_read_byte();    // Read temperature LSB byte and store it on raw_temp_value LSB byte
  *raw_temp_value |= (int16)(ds18b20_read_byte()) << 8;     // Read temperature MSB byte and store it on raw_temp_value MSB byte
  return TRUE;                                // OK --> return 1
}
 
void main() {
  lcd_init();                                    // Initialize LCD module
  lcd_putc('\f');                                // Clear LCD
  lcd_gotoxy(3, 1);                              // Go to column 3 row 1
  printf(lcd_putc, "Temperature:");
  while(TRUE) {
    if(ds18b20_read(&raw_temp)) {
      temp = (float)raw_temp / 16;               // Convert temperature raw value into degree Celsius (temp in °C = raw/16)
      lcd_gotoxy(5, 2);                          // Go to column 5 row 2
      printf(lcd_putc, "%f", temp);
      lcd_putc(223);                             // Print degree symbol ( ° )
      lcd_putc("C ");                            // Print 'C '
    }
    else {
      lcd_gotoxy(5, 2);                          // Go to column 5 row 2
      printf(lcd_putc, " Error! ");
    }
    delay_ms(1000);                              // Wait 1 second
  }
}

*/