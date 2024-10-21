// STM32L432KC_SPI.c
//  Audrey Vo
//  avo@hmc.edu
// 10/18/2024
// SPI functions

#include "STM32L432KC_SPI.h"
#include "STM32L432KC_RCC.h"

/* Enables the SPI peripheral and intializes its clock speed (baud rate), polarity, and phase.
 *    -- br: (0b000 - 0b111). The SPI clk will be the master clock / 2^(BR+1).
 *    -- cpol: clock polarity (0: inactive state is logical 0, 1: inactive state is logical 1).
 *    -- cpha: clock phase (0: data captured on leading edge of clk and changed on next edge, 
 *          1: data changed on leading edge of clk and captured on next edge)
 * Refer to the datasheet for more low-level details. */ 
 
void initSPI(int br, int cpol, int cpha) {
  SPI1->CR1 |= (0b010 << SPI_CR1_BR_Pos); // divide by 8
  SPI1->CR1 |= _VAL2FLD(SPI_CR1_CPOL, cpol); 
  SPI1->CR1 |= _VAL2FLD(SPI_CR1_CPHA, cpha);
  SPI1->CR1 |= _VAL2FLD(SPI_CR1_RXONLY, 0b0);
  SPI1->CR1 |= _VAL2FLD(SPI_CR1_LSBFIRST, 0b0);
  SPI1->CR1 |= _VAL2FLD(SPI_CR1_CRCEN, 0b0);
  SPI1->CR1 |= _VAL2FLD(SPI_CR1_SSM, 0b1); // enable slave software
   SPI1->CR1 |= _VAL2FLD(SPI_CR1_SSI, 0b1); // enable slave software
  SPI1->CR1 |= _VAL2FLD(SPI_CR1_MSTR, 0b1); // set to master


  SPI1->CR2 |= (0b0111 << SPI_CR2_DS_Pos); // set data size to 8 bits
  SPI1->CR2 |= _VAL2FLD(SPI_CR2_SSOE, 0b1);
  SPI1->CR2 |= _VAL2FLD(SPI_CR2_FRF, 0b0); // motorola mode
  SPI1->CR2 |= _VAL2FLD(SPI_CR2_FRXTH, 0b1); // motorola mode

  SPI1->CR1 |= _VAL2FLD(SPI_CR1_SPE, 0b1); // set to master

}

/* Transmits a character (1 byte) over SPI and returns the received character.
 *    -- send: the character to send over SPI
 *    -- return: the character received over SPI */ 
char spiSendReceive(char send){
  while(!(SPI_SR_TXE & SPI1->SR)) {
  }
  //data is sent because it is written to data register
  volatile uint8_t *draddress = &SPI1->DR;
  *draddress = send;
  // checks that data has been received
  while(!(SPI_SR_RXNE & SPI1->SR)) {
  }
  // gets the receive data
  return *draddress;


} 