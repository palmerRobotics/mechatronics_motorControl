#ifndef POSITIONCONTROL__H__
#define POSOTIONCONTROL__H__

typedef struct{
    float kp;
    float ki;
    float kd;
} positionGains_t;

void initPositionControl();
void setPositionGains(positionGains_t gains);
positionGains_t getPositionGains();

#endif