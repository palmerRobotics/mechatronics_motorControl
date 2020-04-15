#ifndef CURRENTCONTROL__H__
#define CURRENTCONTROL__H__

typedef struct {
    float kp;
    float ki;
} currentGains_t; //!!do I want this struct to be visible outside currentControl.h?

void initIcontrol();
void setFixedPWM(int dutyCycle);
void setCurrent(); //unsure of functionality
void setCurrentGains(currentGains_t gains);
currentGains_t getCurrentGains(); 

#endif