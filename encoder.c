#include "encoder.h"

#define COUNTS_PER_DEGREE 2
void init(){
    //initialize encoder
}

int getEncoderCount(){
    //get encoder count
    int count = 1023;
    return count;
}

int getEncoderDegrees(){
    int count = getEncoderCount();
    return countToDegrees(count);
}

int countToDegrees(int count){
    return count/COUNTS_PER_DEGREE;
}

void reset(){
    
}