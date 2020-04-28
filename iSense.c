#include "iSense.h"
#include "stdio.h"
#include "fir.h"
#include <xc.h>
#include "stdio.h"
#include "NU32.h"

#define SAMPLE_AVERAGE 3
#define MAMPS_PER_COUNT 3.815
#define ZERO_CURRENT_COUNT 508
#define SAMPLE_TIME 10          // 10 core timer ticks = 250 ns

#define BUF_SIZE 200

static char buffer[BUF_SIZE];
static unsigned int prevCount = 508;

void initADC(){
  AD1PCFGbits.PCFG9 = 0;       // B9 is an adc pin
  AD1CON3bits.ADCS = 2;        // ADC clock period is Tad = 2*(ADCS+1)*Tpb = 2*3*12.5ns = 75ns              
  AD1CHSbits.CH0SA = 9;        //B9 is positive input of ADC
  AD1CON1bits.ADON = 1;        //enable ADC
}

unsigned int sampleADC(){
  _CP0_SET_COUNT(0);
  AD1CON1bits.SAMP = 1;
  while (_CP0_GET_COUNT() < SAMPLE_TIME) { //maybe not enough time for sampling?
    ;                                   // sample for more than 250 ns
  }
  AD1CON1bits.SAMP = 0;
  while(!AD1CON1bits.DONE){             // wait for conversion to finish
    ;
  }
  return ADC1BUF0;
}

unsigned int getADCcount(){
  int i = 0;
  unsigned int totalCount = 0;
  for(i = 0; i < SAMPLE_AVERAGE; i++){
    totalCount += sampleADC();
  }
  return totalCount/SAMPLE_AVERAGE;
  /*unsigned int count = ADC1BUF0;
  if(count - prevCount  > 100){
    count = prevCount - (prevCount-count)/2;
  }
  else if(count - prevCount < -100){
    count = prevCount - (prevCount-count)/2;
  }
  prevCount = count;
  return count;*/
  return ADC1BUF0;
}

float countToMA(unsigned int count){
  float milliamps = ((float)count - (float)ZERO_CURRENT_COUNT)*(float)MAMPS_PER_COUNT;
  //sprintf(buffer, "mA: %f\r\n", milliamps);
  //NU32_WriteUART3(buffer);
  return milliamps;
}

float getCurrentmAmps(){
  unsigned int count = getADCcount();
  //sprintf(buffer, "cnt: %u ", count);
  //NU32_WriteUART3(buffer);
  return countToMA(count);
}
