#include "positionControl.h"

static positionGains_t posGains;

void initPositionControl(){

}


void setPositionGains(positionGains_t gains){
    posGains = gains;
}

positionGains_t getPositionGains(){
    return posGains;
}