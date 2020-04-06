#include "currentSense.h"

#define AMPS_PER_COUNT (3.3/1024) //!! is this correct?
#define A_TO_MA 1000

void init(){
    
}

int getCount(){
    int count = 0;
    //get value from ADC, write to count
    return count;
}

int getCurrent(){
    int count = getCount();
    int current = countToMA(count);
}

int countToMA(int count){
    return count*AMPS_PER_COUNT*A_TO_MA; //probably incorrect
}