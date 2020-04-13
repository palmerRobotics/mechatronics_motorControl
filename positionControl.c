#include "positionControl.h"

static positionGains posGains;

void initPositionControl(){

}


void setPositionGains(positionGains gains){
    posGains = gains;
}

positionGains getPositionGains(){
    return posGains;
}