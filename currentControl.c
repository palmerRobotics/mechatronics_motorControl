#include "currentControl.h"

static currentGains iGains;

void initIcontrol(){

}

void setFixedPWM(int dutyCycle){

}

void setCurrent(){

}
void setCurrentGains(currentGains input){
    iGains = input;
}
currentGains getCurrentGains(){
    return iGains;
}