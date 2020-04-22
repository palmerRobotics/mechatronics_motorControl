#include "iSense.h"
#include "stdio.h"
#include "fir.h"
#include <xc.h>

#define SAMPLE_AVERAGE 5
#define MAMPS_PER_COUNT 3.815
#define ZERO_CURRENT_COUNT 504
#define SAMPLE_TIME 10          // 10 core timer ticks = 250 ns

#define BUF_SIZE 200

static char buffer[BUF_SIZE];

void initADC(){
  AD1PCFGbits.PCFG9 = 0;       // B9 is an adc pin
  AD1CON3bits.ADCS = 2;        // ADC clock period is Tad = 2*(ADCS+1)*Tpb = 2*3*12.5ns = 75ns
    //AD1CON1bits.SSRC = 0b111;    // 0 //auto-convert after sampling is finished
    //AD1CON1bits.ASAM = 0;        //do not automatically sample after conversion is over  //TRY AUTOMATIC SAMPLING               
  AD1CHSbits.CH0SA = 9;        //B9 is positive input of ADC
  AD1CON1bits.ADON = 1;        //enable ADC
}

int sampleADC(){
  _CP0_SET_COUNT(0);
  AD1CON1bits.SAMP = 1;
  while (_CP0_GET_COUNT() < SAMPLE_TIME) { 
    ;                                   // sample for more than 250 ns
  }
    AD1CON1bits.SAMP = 0;
  while(!AD1CON1bits.DONE){
    ;
  }
  return ADC1BUF0;
}

float countToMA(int count){
  float milliamps = (count -  ZERO_CURRENT_COUNT)*MAMPS_PER_COUNT;
  return milliamps;
}

int getADCcount(){
  int i =0;
  int totalCounts = 0;
  for(i = 0; i < SAMPLE_AVERAGE; i++){
    totalCounts += sampleADC();
  }
  return totalCounts/SAMPLE_AVERAGE;
}

float getCurrentmAmps(){
  int count = getADCcount();
  return countToMA(count);
}
