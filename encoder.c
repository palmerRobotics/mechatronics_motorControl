#include "encoder.h"
#include "NU32.h"

#define COUNTS_PER_ROTATION 4735
#define ZERO_POSITION_COUNT 32768

static char buffer[200];
void initEncoder(){ // SPI initialization for reading from the decoder chip
  AD1PCFGbits.PCFG8 = 1;       // B8 is an digital i/o
  TRISBbits.TRISB8 = 0;     // set B8 low
  SPI4CON = 0;              // stop and reset SPI4
  SPI4BUF;                  // read to clear the rx receive buffer
  SPI4BRG = 0x4;            // bit rate to 8 MHz, SPI4BRG = 80000000/(2*desired)-1
  SPI4STATbits.SPIROV = 0;  // clear the overflow
  SPI4CONbits.MSTEN = 1;    // master mode
  SPI4CONbits.MSSEN = 1;    // slave select enable
  SPI4CONbits.MODE16 = 1;   // 16 bit mode
  SPI4CONbits.MODE32 = 0; 
  SPI4CONbits.SMP = 1;      // sample at the end of the clock
  SPI4CONbits.ON = 1;       // turn SPI on
  resetEncoder();
}

static int prev_count = 0;

static int encoder_command(int read) { // send a command to the encoder chip
                                       // 0 = reset count to 32,768, 1 = return the count
  SPI4BUF = read;                      // send the command
  while (!SPI4STATbits.SPIRBF) { ; }   // wait for the response
  SPI4BUF;                             // garbage was transferred, ignore it
  SPI4BUF = 5;                         // write garbage, but the read will have the data
  while (!SPI4STATbits.SPIRBF) { ; }
  return SPI4BUF;
}

float countToDegrees(int count){
    return (float)-1*((count - ZERO_POSITION_COUNT)*360)/(float)COUNTS_PER_ROTATION;
}

int getEncoderCount(){
  encoder_command(1); //read twice due to bug in code of dsPIC
  return encoder_command(1);
}

float getEncoderDegrees(){
  return countToDegrees(getEncoderCount());
}

void resetEncoder(){
    encoder_command(0);
    return;
}