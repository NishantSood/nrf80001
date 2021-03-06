/*********************************************************************
This is a library for our nRF8001 Bluetooth Low Energy Breakout

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/1697

These displays use SPI to communicate, 4 or 5 pins are required to  
interface

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Kevin Townsend/KTOWN  for Adafruit Industries.  
MIT license, check LICENSE for more information
All text above, and the splash screen below must be included in any redistribution
*********************************************************************/
#define F_CPU 8000000UL
#include "SPI.h"
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdlib.h>
#include "utility/ble_system.h"
#include "utility/lib_aci.h"
#include "utility/aci_setup.h"
#include "utility/uart/services.h"
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>

#include "Adafruit_BLE_UART.h"

/* Get the service pipe data created in nRFGo Studio */
#ifdef SERVICES_PIPE_TYPE_MAPPING_CONTENT
    static services_pipe_type_mapping_t
        services_pipe_type_mapping[NUMBER_OF_PIPES] = SERVICES_PIPE_TYPE_MAPPING_CONTENT;
#else
    #define NUMBER_OF_PIPES 0
    static services_pipe_type_mapping_t * services_pipe_type_mapping = NULL;
#endif

/* Store the setup for the nRF8001 in the flash of the AVR to save on RAM */
static const hal_aci_data_t setup_msgs[NB_SETUP_MESSAGES]PROGMEM = SETUP_MESSAGES_CONTENT; // this was static hal_aci_data_t setup_msgs[NB_SETUP_MESSAGES] PROGMEM = SETUP_MESSAGES_CONTENT;

static struct aci_state_t aci_state;            /* ACI state data */
static hal_aci_evt_t  aci_data;                 /* Command buffer */
static bool timing_change_done = false;

static uint8_t uart_buffer[20];
static uint8_t uart_buffer_len = 0;
static char device_name[11] = "nish";

// This is the Uart RX buffer, which we manage internally when data is available!
#define ADAFRUIT_BLE_UART_RXBUFFER_SIZE 64
uint8_t adafruit_ble_rx_buffer[ADAFRUIT_BLE_UART_RXBUFFER_SIZE];
volatile uint16_t adafruit_ble_rx_head;
volatile uint16_t adafruit_ble_rx_tail;


uint8_t HAL_IO_RADIO_RESET, HAL_IO_RADIO_REQN, HAL_IO_RADIO_RDY, HAL_IO_RADIO_IRQ;

/**************************************************************************/
/*!
    Constructor for the UART service
*/
/**************************************************************************/
// default RX callback!

void Adafruit_BLE_UART::defaultRX(uint8_t *buffer, uint8_t len)
{
  for(int i=0; i<len; i++)
  {
    uint16_t new_head = (uint16_t)(adafruit_ble_rx_head + 1) % ADAFRUIT_BLE_UART_RXBUFFER_SIZE;
    
    // if we should be storing the received character into the location
    // just before the tail (meaning that the head would advance to the
    // current location of the tail), we're about to overflow the buffer
    // and so we don't write the character or advance the head.
    if (new_head != adafruit_ble_rx_tail) {
      adafruit_ble_rx_buffer[adafruit_ble_rx_head] = buffer[i];

      // debug echo print
      // Serial.print((char)buffer[i]); 

      adafruit_ble_rx_head = new_head;
    }
  }

  /*
  Serial.print("Buffer: ");
  for(int i=0; i<adafruit_ble_rx_head; i++)
    {
      Serial.print(" 0x"); Serial.print((char)adafruit_ble_rx_buffer[i], HEX); 
    }
  Serial.println();
  */
}

void Adafruit_BLE_UART::ble_set_name(char *name)
{       
    unsigned char len=0;
    
    len = strlen(name);

    if(len > 10){
        //Serial.print(F("the new name is too long"));        
   } else{
       strcpy(device_name, name);
   }
}

/* Stream stuff */

int Adafruit_BLE_UART::available(void)
{
  return (uint16_t)(ADAFRUIT_BLE_UART_RXBUFFER_SIZE + adafruit_ble_rx_head - adafruit_ble_rx_tail) 
    % ADAFRUIT_BLE_UART_RXBUFFER_SIZE;
}

int Adafruit_BLE_UART::read(void)
{
  // if the head isn't ahead of the tail, we don't have any characters
  if (adafruit_ble_rx_head == adafruit_ble_rx_tail) {
    return -1;
  } else {
    unsigned char c = adafruit_ble_rx_buffer[adafruit_ble_rx_tail];
    adafruit_ble_rx_tail ++;
    adafruit_ble_rx_tail %= ADAFRUIT_BLE_UART_RXBUFFER_SIZE;
    return c;
  }
}

int Adafruit_BLE_UART::peek(void)
{
  if (adafruit_ble_rx_head ==  adafruit_ble_rx_tail) {
    return -1;
  } else {
    return adafruit_ble_rx_buffer[adafruit_ble_rx_tail];
  }
}

void Adafruit_BLE_UART::flush(void)
{
  // MEME: KTOWN what do we do here?
}



//// more callbacks

void Adafruit_BLE_UART::defaultACICallback(aci_evt_opcode_t event)
{
  currentStatus = event;
}

aci_evt_opcode_t Adafruit_BLE_UART::getState(void) {
  return currentStatus;
}



/**************************************************************************/
/*!
    Constructor for the UART service
*/
/**************************************************************************/
Adafruit_BLE_UART::Adafruit_BLE_UART(int8_t req, int8_t rdy, int8_t rst)
{
  debugMode = true;
  
  HAL_IO_RADIO_REQN = req;
  HAL_IO_RADIO_RDY = rdy;
  HAL_IO_RADIO_RESET = rst;

  rx_event = NULL;
  aci_event = NULL;

  adafruit_ble_rx_head = adafruit_ble_rx_tail = 0;

  currentStatus = ACI_EVT_DISCONNECTED;
}

void Adafruit_BLE_UART::setACIcallback(aci_callback aciEvent) {
  aci_event = aciEvent;
}

void Adafruit_BLE_UART::setRXcallback(rx_callback rxEvent) {
  rx_event = rxEvent;
}

/**************************************************************************/
/*!
    Transmits data out via the TX characteristic (when available)
*/
/**************************************************************************/
uint16_t Adafruit_BLE_UART::println(const char * thestr) 
{
  char buffer[20] = { 0 };
  size_t len = strlen(thestr);
  
  if ((len) > 18) return 0;
  
  memcpy(buffer, thestr, len);
  buffer[len] = '\n';
  buffer[len+1] = '\r';
  
  uint16_t written = print(buffer);
  return written;
}

uint16_t Adafruit_BLE_UART::print(const char * thestr)
{
  uint16_t written = 0;
  uint8_t intbuffer[20];

  while (strlen(thestr) > 20) {
    strncpy((char *)intbuffer, thestr, 20);
    written += write(intbuffer, 20);
    thestr += 20;
  }
  strncpy((char *)intbuffer, thestr, 20);
  written += write(intbuffer, strlen(thestr));
 
  return written;
}


uint16_t Adafruit_BLE_UART::write(uint8_t * buffer, uint8_t len)
{
  /* ToDo: handle packets > 20 bytes in multiple transmits! */
  if (len > 20)
  {
    len = 20;
  }

#ifdef BLE_RW_DEBUG
  //Serial.print(F("\tWriting out to BTLE:"));
  for (uint8_t i=0; i<len; i++) {
    //Serial.print(F(" 0x")); Serial.print(buffer[i], HEX);
  }
  //Serial.println();
#endif

  if (lib_aci_is_pipe_available(&aci_state, PIPE_UART_OVER_BTLE_UART_TX_TX))
  {
    lib_aci_send_data(PIPE_UART_OVER_BTLE_UART_TX_TX, buffer, len);
    aci_state.data_credit_available--;

    _delay_ms(10); // required 10ms delay between sends
    return len;
  }

  pollACI();
  
  return 0;
}

uint16_t Adafruit_BLE_UART::write(uint8_t buffer)
{
#ifdef BLE_RW_DEBUG
  //Serial.print(F("\tWriting one byte 0x")); Serial.println(buffer, HEX);
#endif
  if (lib_aci_is_pipe_available(&aci_state, PIPE_UART_OVER_BTLE_UART_TX_TX))
  {
    lib_aci_send_data(PIPE_UART_OVER_BTLE_UART_TX_TX, &buffer, 1);
    aci_state.data_credit_available--;

    _delay_ms(50); // required 10ms delay between sends
    return 1;
  }
  
  pollACI();
  
  return 0;
}


/**************************************************************************/
/*!
    Handles low level ACI events, and passes them up to an application
    level callback when appropriate
*/
/**************************************************************************/
void Adafruit_BLE_UART::pollACI()
{
  // We enter the if statement only when there is a ACI event available to be processed
  if (lib_aci_event_get(&aci_state, &aci_data))
  {
    aci_evt_t * aci_evt;
    
    aci_evt = &aci_data.evt;    
    switch(aci_evt->evt_opcode)
    {
        /* As soon as you reset the nRF8001 you will get an ACI Device Started Event */
        case ACI_EVT_DEVICE_STARTED:
        {          
          aci_state.data_credit_total = aci_evt->params.device_started.credit_available;
          switch(aci_evt->params.device_started.device_mode)
          {
            case ACI_DEVICE_SETUP:
            /* Device is in setup mode! */
            if (ACI_STATUS_TRANSACTION_COMPLETE != do_aci_setup(&aci_state))
            {
              if (debugMode) {
                //Serial.println(F("Error in ACI Setup"));
              }
            }
            break;
            
            case ACI_DEVICE_STANDBY:
              /* Start advertising ... first value is advertising time in seconds, the */
              /* second value is the advertising interval in 0.625ms units */
              lib_aci_connect(adv_timeout, adv_interval);
              defaultACICallback(ACI_EVT_DEVICE_STARTED);
	      if (aci_event) 
		aci_event(ACI_EVT_DEVICE_STARTED);
          }
        }
        break;
        
      case ACI_EVT_CMD_RSP:
        /* If an ACI command response event comes with an error -> stop */
        if (ACI_STATUS_SUCCESS != aci_evt->params.cmd_rsp.cmd_status)
        {
          // ACI ReadDynamicData and ACI WriteDynamicData will have status codes of
          // TRANSACTION_CONTINUE and TRANSACTION_COMPLETE
          // all other ACI commands will have status code of ACI_STATUS_SUCCESS for a successful command
          if (debugMode) {
			/*
            Serial.print(F("ACI Command "));
            Serial.println(aci_evt->params.cmd_rsp.cmd_opcode, HEX);
            Serial.println(F("Evt Cmd respone: Error. Arduino is in an while(1); loop"));
			*/
          }
          while (1);
        }
        if (ACI_CMD_GET_DEVICE_VERSION == aci_evt->params.cmd_rsp.cmd_opcode)
        {
          // Store the version and configuration information of the nRF8001 in the Hardware Revision String Characteristic
          lib_aci_set_local_data(&aci_state, PIPE_DEVICE_INFORMATION_HARDWARE_REVISION_STRING_SET, 
            (uint8_t *)&(aci_evt->params.cmd_rsp.params.get_device_version), sizeof(aci_evt_cmd_rsp_params_get_device_version_t));
        }        
        break;
        
      case ACI_EVT_CONNECTED:
        aci_state.data_credit_available = aci_state.data_credit_total;
        /* Get the device version of the nRF8001 and store it in the Hardware Revision String */
        lib_aci_device_version();
        
	defaultACICallback(ACI_EVT_CONNECTED);
	if (aci_event) 
	  aci_event(ACI_EVT_CONNECTED);
        
      case ACI_EVT_PIPE_STATUS:
        if (lib_aci_is_pipe_available(&aci_state, PIPE_UART_OVER_BTLE_UART_TX_TX) && (false == timing_change_done))
        {
          lib_aci_change_timing_GAP_PPCP(); // change the timing on the link as specified in the nRFgo studio -> nRF8001 conf. -> GAP. 
                                            // Used to increase or decrease bandwidth
          timing_change_done = true;
        }
        break;
        
      case ACI_EVT_TIMING:
        /* Link connection interval changed */
        break;
        
      case ACI_EVT_DISCONNECTED:
        /* Restart advertising ... first value is advertising time in seconds, the */
        /* second value is the advertising interval in 0.625ms units */

	defaultACICallback(ACI_EVT_DISCONNECTED);
	if (aci_event)
	  aci_event(ACI_EVT_DISCONNECTED);

	lib_aci_connect(adv_timeout, adv_interval);

	defaultACICallback(ACI_EVT_DEVICE_STARTED);
	if (aci_event)
	  aci_event(ACI_EVT_DEVICE_STARTED);
	break;
        
      case ACI_EVT_DATA_RECEIVED:
        for(int i=0; i<aci_evt->len - 2; i++)
        {
          /* Fill uart_buffer with incoming data */
          uart_buffer[i] = aci_evt->params.data_received.rx_data.aci_data[i];
        }
        /* Set the buffer len */
        uart_buffer_len = aci_evt->len - 2;
	defaultRX(uart_buffer, uart_buffer_len);
        if (rx_event)
	  rx_event(uart_buffer, uart_buffer_len);
        break;
   
      case ACI_EVT_DATA_CREDIT:
        aci_state.data_credit_available = aci_state.data_credit_available + aci_evt->params.data_credit.credit;
        break;
      
      case ACI_EVT_PIPE_ERROR:
        /* See the appendix in the nRF8001 Product Specication for details on the error codes */
        if (debugMode) {
		/*
          Serial.print(F("ACI Evt Pipe Error: Pipe #:"));
          Serial.print(aci_evt->params.pipe_error.pipe_number, DEC);
          Serial.print(F("  Pipe Error Code: 0x"));
          Serial.println(aci_evt->params.pipe_error.error_code, HEX);
		*/
        }

        /* Increment the credit available as the data packet was not sent */
        aci_state.data_credit_available++;
        break;
    }
  }
  else
  {
    // Serial.println(F("No ACI Events available"));
    // No event in the ACI Event queue and if there is no event in the ACI command queue the arduino can go to sleep
    // Arduino can go to sleep now
    // Wakeup from sleep from the RDYN line
	  
	 //LowPower.idle(SLEEP_FOREVER, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_ON, SPI_ON, USART0_OFF, TWI_ON); 
	 //LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF); 
	  // disable ADC
	  /*
       ADCSRA = 0;     
       set_sleep_mode (SLEEP_MODE_PWR_DOWN);  
       sleep_enable();
 
       // turn off brown-out enable in software
       MCUCR = bit (BODS) | bit (BODSE);
       MCUCR = bit (BODS); 
       sleep_cpu (); 
      */	   
  }
}

/**************************************************************************/
/*!
    Configures the nRF8001 and starts advertising the UART Service
    
    @param[in]  advTimeout  
                The advertising timeout in seconds (0 = infinite advertising)
    @param[in]  advInterval
                The delay between advertising packets in 0.625ms units
*/
/**************************************************************************/
bool Adafruit_BLE_UART::begin(uint16_t advTimeout, uint16_t advInterval) 
{
  /* Store the advertising timeout and interval */
  adv_timeout = advTimeout;   /* ToDo: Check range! */
  adv_interval = advInterval; /* ToDo: Check range! */
  
  /* Setup the service data from nRFGo Studio (services.h) */
  if (NULL != services_pipe_type_mapping)
  {
    aci_state.aci_setup_info.services_pipe_type_mapping = &services_pipe_type_mapping[0];
  }
  else
  {
    aci_state.aci_setup_info.services_pipe_type_mapping = NULL;
  }
  aci_state.aci_setup_info.number_of_pipes    = NUMBER_OF_PIPES;
  aci_state.aci_setup_info.setup_msgs         = (hal_aci_data_t*) setup_msgs;
  aci_state.aci_setup_info.num_setup_msgs     = NB_SETUP_MESSAGES;

  /* Pass the service data into the appropriate struct in the ACI */
  lib_aci_init(&aci_state);

  /* ToDo: Check for chip ID to make sure we're connected! */
  
  return true;
}
