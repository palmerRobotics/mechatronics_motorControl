#include "iSense.h"

#define AMPS_PER_COUNT (3.3/1024) //!! is this correct?
#define A_TO_MA 1000

void initADC(){
    //initialize ADC
}

int getADCcount(){
    int count = 1023; //dummy variable for testing
    //get value from ADC, write to count
    return count;
}

int getCurrentmAmps(){
    int count = getADCcount();
    return countToMA(count);
}

int countToMA(int count){
    return count*AMPS_PER_COUNT*A_TO_MA; //probably incorrect
}