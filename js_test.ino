#include "SPI.h"
#include <Joystick.h>

#define spi_SCK  13
#define spi_MISO 12
#define spi_PCS  10
#define spi_INT   7

volatile unsigned int  spi_msw; // most significant word
volatile unsigned int  spi_lsw; // least significant word
volatile unsigned long spi_dat; // full 32 bit word
volatile boolean spi_new;       // new spi data available

// ---------------------------------------------------
// 
// ---------------------------------------------------
void setup() {

  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  
  pinMode(spi_MISO, INPUT_PULLUP);  // MISO
  pinMode(spi_SCK, OUTPUT);         // SCK
  pinMode(spi_PCS, OUTPUT);         // PCS

  digitalWrite(spi_SCK, LOW);
  digitalWrite(spi_PCS, LOW);

  Joystick.begin();

  read_keys();     // clear interrupt
  spi_new = false; // reset new data flag
  attachInterrupt(digitalPinToInterrupt(spi_INT), read_keys, RISING);

  Serial.println ("Setup is done.");  

} // setup

// ---------------------------------------------------
// 
// ---------------------------------------------------
void loop() {

  if (spi_new) {

    spi_new = false;
    updateButtons();
    
  } // if

} // main

// ---------------------------------------------------
// interrupt routine, must be void
// ---------------------------------------------------
void read_keys() {

  digitalWrite(spi_PCS, HIGH);
  spi_msw = SPI.transfer16(0);
  spi_lsw = SPI.transfer16(0);
  digitalWrite(spi_PCS, LOW);

  spi_dat = spi_msw;
  spi_dat <<= 16;
  spi_dat += spi_lsw;

  spi_new = true;

} // read_keys

// ---------------------------------------------------
// 
// ---------------------------------------------------
void updateButtons() {
  unsigned char i;

  for (i=0; i<32; i++, spi_dat>>=1)
    if (spi_dat & 1)
      Joystick.setButton(i, 1);
    else
      Joystick.setButton(i, 0);

} // print_keys
