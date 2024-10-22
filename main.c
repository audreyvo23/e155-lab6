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
#include "stm32l432xx.h"

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
char* tempResol = "<p>Temperature Resolution:</p><form action=\"8bit\"><input type=\"submit\" value=\"8-bit Resolution\"></form>\
	<form action=\"9bit\"><input type=\"submit\" value=\"9-bit Resolution\"></form>\
        <form action=\"10bit\"><input type=\"submit\" value=\"10-bit Resolution\"></form>\
        <form action=\"11bit\"><input type=\"submit\" value=\"11-bit Resolution\"></form>\
        <form action=\"12bit\"><input type=\"submit\" value=\"12-bit Resolution\"></form>";

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

int res_status = 8;
int updateTempRes(char request[])
{
	// The request has been received. now process to determine whether to turn the LED on or off
	if (inString(request, "8bit")==1) {
		res_status = 8;
	}
	else if (inString(request, "9bit")==1) {
		res_status = 9;
	}
        else if (inString(request, "10bit")==1) {
		res_status = 10;
	}

        else if (inString(request, "11bit")==1) {
		res_status = 11;
	}

        else if (inString(request, "12bit")==1) {
		res_status = 12;
	}

	return res_status;
}

// Function used by printf to send characters to the laptop
int _write(int file, char *ptr, int len) {
  int i = 0;
  for (i = 0; i < len; i++) {
    ITM_SendChar((*ptr++));
  }
  return len;
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

    // Receive web request from the ESP
    char request[BUFF_LEN] = "                  "; // initialize to known value
    int charIndex = 0;

    // Keep going until you get end of line character
    while(inString(request, "\n") == -1) {
      // Wait for a complete request to be transmitted before processing
      while(!(USART->ISR & USART_ISR_RXNE));
      request[charIndex++] = readChar(USART);
    } 
    

    // Add SPI code here for reading temperature
  
    // Update string with current LED state
   
    // spi write data 
    int msb;
    char lsb;
    char resol;
    res_status = updateTempRes(request);

    // set the resolution
    if(res_status == 8) resol = 0b11100000;
    if(res_status == 9) resol = 0b11100010;
    if(res_status == 10) resol = 0b11100100;
    if(res_status == 11) resol = 0b11100110;
    if(res_status == 12) resol = 0b11101000;
    
    digitalWrite(PA8, PIO_HIGH);
    spiSendReceive(0x80);
    spiSendReceive(resol);
   
    digitalWrite(PA8, PIO_LOW);
    digitalWrite(PA8, PIO_HIGH);
    // spi read data

    spiSendReceive(0x02);
    msb = spiSendReceive(0x00);

    digitalWrite(PA8, PIO_LOW);
    digitalWrite(PA8, PIO_HIGH);

    spiSendReceive(0x01);
    lsb = spiSendReceive(0x00);

    digitalWrite(PA8, PIO_LOW);

    
    delay_millis(TIM15, 100);

   
    // msk msb bit
    int mskmsb = msb & 0b01111111;
    float newtempdata;
    
    //get sign bit
    int signbit = msb & (1 << 7);
    if (!signbit) { // check if positive
      newtempdata = mskmsb;
      if(1 << 7 & lsb) newtempdata += 0.5;
      if(1 << 6 & lsb) newtempdata += 0.25;
      if(1 << 5 & lsb) newtempdata += 0.125;
      if(1 << 4 & lsb) newtempdata += 0.0625; 
    }
    else { // is negative
      newtempdata = -(~mskmsb + 1);
      if(1 << 7 & lsb) newtempdata -= 0.5;
      if(1 << 6 & lsb) newtempdata -= 0.25;
      if(1 << 5 & lsb) newtempdata -= 0.125;
      if(1 << 4 & lsb) newtempdata -= 0.0625;
    }

    printf("msb: %d \n", msb);
    printf("lsb: %d \n", lsb);
    printf("temp: %f \n", newtempdata);
  
    int led_status = updateLEDStatus(request);

    char ledStatusStr[20];
    if (led_status == 1)
      sprintf(ledStatusStr,"LED is on!");
    else if (led_status == 0)
      sprintf(ledStatusStr,"LED is off!");

    char temp[20];
    char units[20];
    sprintf(temp, "%f ", newtempdata);
    sprintf(units,"degrees Celsius");

    // finally, transmit the webpage over UART
    sendString(USART, webpageStart); // webpage header code
    sendString(USART, ledStr); // button for controlling LED

    sendString(USART, "<h2>LED Status</h2>");


    sendString(USART, "<p>");
    sendString(USART, ledStatusStr);
    sendString(USART, "</p>");

    sendString(USART, "<h2>Current Temperature</h2>");
    sendString(USART, "</p>");
    sendString(USART, temp);
    sendString(USART, units);
    sendString(USART, "</p>");
    sendString(USART, tempResol);
    sendString(USART, webpageEnd);
    
    
  }
}
