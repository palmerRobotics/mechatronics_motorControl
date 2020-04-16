#include "currentControl.h"
#include "xc.h"
#include <sys/attribs.h> 

static currentGains_t iGains;
static volatile int mode; //should this be static?

void __ISR(_TIMER_3_VECTOR, IPL6SRS) currentISR(void){
    //check mode: (follow two sets are dependent on mode)
        //set OC2RS
        //set direction
    //check current sensor
        //adjust PWM dependent on iActual (also dependend on mode)
    LATDbits.LATD1 = 1;
    LATFINV = 0x1;
    IFS0bits.T3IF = 0; //clear interrupt flag
}

void initIcontrol(){
    
    __builtin_disable_interrupts();
    //initialize and enable current control timer
    PR3 = 7999;
    TMR3 = 0;
    T3CONbits.TCKPS = 1;
    T3CONbits.TGATE = 0;
    //T3CONbits.TCS = 0;
    T3CONbits.ON = 1;  //enable timer3
    IPC3bits.T3IP = 6; //priority level 6
    IPC3bits.T3IS = 0; //subpriority 0
    IFS0bits.T3IF = 0; //clear interrupt flag
    IEC0bits.T3IE = 1; //enable interrupt
    __builtin_enable_interrupts();
    

    //initialize and enable PWM timer
    PR2 = 1999;
    TMR2 = 0;
    T2CONbits.TCKPS = 1;
    OC1CONbits.OCM = 0b110;
    OC1RS = 1000; //1000 = 50% duty cycle
    OC1R = 000;
    T2CONbits.ON = 1;
    OC1CONbits.ON = 1;
    
    TRISDbits.TRISD1 = 0; //set D1 as a digital out pin
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