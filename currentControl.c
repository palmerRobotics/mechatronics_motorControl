#include "currentControl.h"

static currentGains_t iGains;

void initIcontrol(){

}

void setFixedPWM(int dutyCycle){

}

void setCurrent(){

}
void setCurrentGains(currentGains_t gains){
    iGains = gains;
}
currentGains_t getCurrentGains(){
    return iGains;
}