#include "positionControl.h"
#include "xc.h"
#include <sys/attribs.h> 

static positionGains_t posGains;

void initPositionControl(){
    __builtin_disable_interrupts();
    //initialize and enable current control timer
    PR4 = 24999;
    TMR4 = 0;
    T4CONbits.TCKPS = 1;
    T4CONbits.TGATE = 0;
    //T4CONbits.TCS = 0;
    T4CONbits.ON = 1;  //enable timer3
    //!!!needs to be changed to timer 4!
    IPC3bits.T3IP = 6; //priority level 6
    IPC3bits.T3IS = 0; //subpriority 0
    IFS0bits.T3IF = 0; //clear interrupt flag
    IEC0bits.T3IE = 1; //enable interrupt
    __builtin_enable_interrupts();
}


void setPositionGains(positionGains_t gains){
    posGains = gains;
}

positionGains_t getPositionGains(){
    return posGains;
}