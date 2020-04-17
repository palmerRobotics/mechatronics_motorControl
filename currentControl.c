#include "currentControl.h"
#include "utilities.h"
#include "xc.h"
#include <sys/attribs.h> 
#include "stdio.h"
#include "NU32.h"

#define PERCENT_TO_DECIMAL 100
#define FORWARD 0
#define REVERSE 1

static currentGains_t iGains;
static float iDesired;
static volatile int dutyCount = 0;
static volatile direction;

static char buf[200];

void __ISR(_TIMER_3_VECTOR, IPL6SRS) currentISR(void){
    //check mode: (follow two sets are dependent on mode)
        //set OC2RS
        //set direction
    //check current sensor
        //adjust PWM dependent on iActual (also dependend on mode)
        switch(getMode()){
            case IDLE:
            {
                NU32_LED1 = 1;
                NU32_LED2 = 0;
                OC1RS = 0;//it seems like pwm isn't going to 0 when using the P command
                dutyCount = 0;
                break;
            }
            case PWM:
            {
                NU32_LED1 = 0;
                NU32_LED2 = 1;
                OC1RS = dutyCount;  //!!PWM not resetting for negative duty cycle
                if(direction == FORWARD){
                    LATDbits.LATD1 = FORWARD;
                    NU32_LED1 = 0; //for debugging
                    NU32_LED2 = 1;
                }
                else{
                    LATDbits.LATD1 = REVERSE; //set direction of pwm
                    NU32_LED1 = 1; //for debugging
                    NU32_LED2 = 0;
                }
                break;
            }
            case ITEST:
            {
                break;
            }
            case HOLD:
            {
                break;
            }
            case TRACK:
            {
                break;
            }
            default:
            {
                break;
            }
        }
        int mode = getMode();
    IFS0bits.T3IF = 0; //clear interrupt flag
}

void initIcontrol(){
    direction = FORWARD;
    __builtin_disable_interrupts();
    //initialize and enable current control timer
    PR3 = 15999;
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
    PR2 = 3999;
    TMR2 = 0;
    T2CONbits.TCKPS = 0;
    OC1CONbits.OCM = 0b110;
    OC1RS = 0;
    OC1R = 0;
    T2CONbits.ON = 1;
    OC1CONbits.ON = 1;
    
    TRISDbits.TRISD1 = 0; //set D1 as a digital out pin

    setMode(IDLE);
}

void setPWM(int dutyCycle){
    if(dutyCycle >= 0){
        direction = FORWARD;
        dutyCount = ((PR2 + 1)*dutyCycle)/PERCENT_TO_DECIMAL;
    }
    else{
        direction = REVERSE;
        dutyCount = ((PR2 + 1)*(-1*dutyCycle))/PERCENT_TO_DECIMAL; //we want the absolute value of dutyCount, so times dutyCycle by -1
    }
    if(dutyCount > PR2){
        dutyCount = PR2;
    }
}

void setCurrent(){
    //sets the current to be tracked by the current controller
}

void setCurrentGains(currentGains_t gains){
    iGains = gains;
}
currentGains_t getCurrentGains(){
    return iGains;
}