/*
File: Lab_6_JHB.c
Author: Audrey Vo
Email: avo@hmc.edu
Date: 10/18/2024
*/


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "main.h"

/////////////////////////////////////////////////////////////////
// Provided Constants and Functions
/////////////////////////////////////////////////////////////////

//Defining the web page in two chunks: everything before the current time, and everything after the current time
char* webpageStart = "<!DOCTYPE html><html><head><title>E155 Web Server Demo Webpage</title>\
	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\
	</head>\
	<body><h1>E155 Web Server Demo Webpage</h1>";
char* ledStr = "<p>LED Control:</p><form action=\"ledon\"><input type=\"submit\" value=\"Turn the LED on!\"></form>\
	<form action=\"ledoff\"><input type=\"submit\" value=\"Turn the LED off!\"></form>";
char* webpageEnd   = "</body></html>";

//determines whether a given character sequence is in a char array request, returning 1 if present, -1 if not present
int inString(char request[], char des[]) {
	if (strstr(request, des) != NULL) {return 1;}
	return -1;
}

int updateLEDStatus(char request[])
{
	int led_status = 0;
	// The request has been received. now process to determine whether to turn the LED on or off
	if (inString(request, "ledoff")==1) {
		digitalWrite(LED_PIN, PIO_LOW);
		led_status = 0;
	}
	else if (inString(request, "ledon")==1) {
		digitalWrite(LED_PIN, PIO_HIGH);
		led_status = 1;
	}

	return led_status;
}

/////////////////////////////////////////////////////////////////
// Solution Functions
/////////////////////////////////////////////////////////////////

int main(void) {
  configureFlash();
  configureClock();

  gpioEnable(GPIO_PORT_A);
  gpioEnable(GPIO_PORT_B);
  gpioEnable(GPIO_PORT_C);

  pinMode(PB3, GPIO_OUTPUT);
  pinMode(PA8, GPIO_OUTPUT);
  
  RCC->APB2ENR |= (RCC_APB2ENR_TIM15EN);
  initTIM(TIM15);

  RCC->APB2ENR |= (RCC_APB2ENR_SPI1EN);
  
  USART_TypeDef * USART = initUSART(USART1_ID, 125000);

  // TODO: Add SPI initialization code
  pinMode(PA5, GPIO_ALT); // sck
  pinMode(PA6, GPIO_ALT); // miso
  pinMode(PA12, GPIO_ALT); // mosi



  // set alt function 5 to pin 5,6, and 12
  GPIOA->AFR[0] |= (0b0101 << GPIO_AFRL_AFSEL5_Pos);   //AF5
  GPIOA->AFR[0] |= (0b0101 << GPIO_AFRL_AFSEL6_Pos);  //AF6
  GPIOA->AFR[1] |= (0b0101 << GPIO_AFRH_AFSEL12_Pos);  //AF12

  // initialize spi with 200kHz baud rate and 1 phase 0 polarity
  initSPI(200000, 0, 1); 

  while(1) { 
    /* Wait for ESP8266 to send a request.
    Requests take the form of '/REQ:<tag>\n', with TAG begin <= 10 characters.
    Therefore the request[] array must be able to contain 18 characters.
    */
/*
    // Receive web request from the ESP
    char request[BUFF_LEN] = "                  "; // initialize to known value
    int charIndex = 0;

    // Keep going until you get end of line character
    while(inString(request, "\n") == -1) {
      // Wait for a complete request to be transmitted before processing
      while(!(USART->ISR & USART_ISR_RXNE));
      request[charIndex++] = readChar(USART);
    }
*/
    // TODO: Add SPI code here for reading temperature
  
    // Update string with current LED state
   
    // spi write data 
    char msb;
    char lsb;
    int temp;
    // on
    digitalWrite(PA8, PIO_HIGH);
    spiSendReceive(0x80);
    spiSendReceive(0x22);
    digitalWrite(PA8, PIO_LOW);
    // off
    /*delay_millis(TIM15, 100);
    // on
    digitalWrite(PA8, PIO_HIGH);
    // spi read data
    spiSendReceive(0x02);
    msb = spiSendReceive(0x00);
    digitalWrite(PA8, PIO_LOW);
    // off
    // on
    digitalWrite(PA8, PIO_HIGH);
    spiSendReceive(0x01);
    lsb = spiSendReceive(0x00);
    digitalWrite(PA8, PIO_LOW);
    // off
    
    int allbits = (msb << 8) | lsb;
    int tempdata= allbits & 0b01111111;
    int newtempdata;
    
    //get sign bit
    int signbit = allbits & (1 << 15);
    if (signbit) { // check if positive
    newtempdata = tempdata;
      
    }
    else {
    newtempdata = ~tempdata + 1;
    }

    

  /*
    int led_status = updateLEDStatus(request);

    char ledStatusStr[20];
    if (led_status == 1)
      sprintf(ledStatusStr,"LED is on!");
    else if (led_status == 0)
      sprintf(ledStatusStr,"LED is off!");



    // finally, transmit the webpage over UART
    sendString(USART, webpageStart); // webpage header code
    sendString(USART, ledStr); // button for controlling LED

    sendString(USART, "<h2>LED Status</h2>");


    sendString(USART, "<p>");
    sendString(USART, ledStatusStr);
    sendString(USART, "</p>");

  
    sendString(USART, webpageEnd);
    */
  }
}
