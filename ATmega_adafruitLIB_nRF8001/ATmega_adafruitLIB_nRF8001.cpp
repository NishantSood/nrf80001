/*
 * ATmega_adafruitLIB_nRF8001.cpp
 *
 * Created: 07-02-2015 20:50:13
 *  Author: nishant
 */ 

#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

// This version uses call-backs on the event and RX so there's no data handling in the main loop!

#include "SPI.h"
#include "Adafruit_BLE_UART.h"

#define ADAFRUITBLE_REQ PORTB1 //9
#define ADAFRUITBLE_RDY PORTD2 //2
#define ADAFRUITBLE_RST PORTD4 //4

Adafruit_BLE_UART uart = Adafruit_BLE_UART(ADAFRUITBLE_REQ, ADAFRUITBLE_RDY, ADAFRUITBLE_RST);

/**************************************************************************/
/*!
    This function is called whenever select ACI events happen
*/
/**************************************************************************/
void aciCallback(aci_evt_opcode_t event)
{
  switch(event)
  {
    case ACI_EVT_DEVICE_STARTED:
      //Serial.println(F("Advertising started"));
      break;
    case ACI_EVT_CONNECTED:
      //Serial.println(F("Connected!"));
      break;
    case ACI_EVT_DISCONNECTED:
      //Serial.println(F("Disconnected or advertising timed out"));
      break;
    default:
      break;
  }
}

/**************************************************************************/
/*!
    This function is called whenever data arrives on the RX channel
*/
/**************************************************************************/
void rxCallback(uint8_t *buffer, uint8_t len)
{
  //Serial.print(F("Received "));
  //Serial.print(len);
  //Serial.print(F(" bytes: "));
  for(int i=0; i<len; i++)
   //Serial.print((char)buffer[i]); 

  //Serial.print(F(" ["));

  for(int i=0; i<len; i++)
  {
    //Serial.print(" 0x"); Serial.print((char)buffer[i], HEX); 
  }
  //Serial.println(F(" ]"));

  /* Echo the same data back! */  
  uart.write(buffer, len);
}

int main(void)
{
	sei();
	//EICRA |= (1 << ISC00) | (1 << ISC01) | (1 << ISC10) | (1 << ISC11);
	uart.setRXcallback(rxCallback);
	uart.setACIcallback(aciCallback);
	uart.begin();
    while(1)
    {
		uart.pollACI();
        //TODO:: Please write your application code 
    }
}