#include "iSense.h"
#include "stdio.h"
#include <xc.h>

#define MAMPS_PER_COUNT 3.4
#define ZERO_CURRENT_COUNT 502

void initADC(){
    AD1PCFGbits.PCFG9 = 0;       // B9 is an adc pin
    AD1CON3bits.ADCS = 2;        // ADC clock period is Tad = 2*(ADCS+1)*Tpb = 2*3*12.5ns = 75ns
    AD1CON1bits.SSRC = 0b111;    //auto-convert after sampling is finished
    AD1CON1bits.ASAM = 0;        //do not automatically sample after conversion is over                   
    AD1CHSbits.CH0SA = 9;        //B9 is positive input of ADC
    AD1CON1bits.ADON = 1;        //enable ADC
}

int getADCcount(){
    AD1CON1bits.SAMP = 1;
    while(!AD1CON1bits.DONE){
        ;
    }
   return ADC1BUF0;
}

int getCurrentmAmps(){
    int count = getADCcount();
    return countToMA(count);
}

int countToMA(int count){
    return (count -  ZERO_CURRENT_COUNT)*MAMPS_PER_COUNT; //using Marcos makes it integer math. Need to change to floats if I want decimal values
}