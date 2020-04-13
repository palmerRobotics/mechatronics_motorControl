#ifndef POSITIONCONTROL__H__
#define POSOTIONCONTROL__H__

typedef struct{
    float kp;
    float ki;
    float kd;
} positionGains;

void initPositionControl();
void setPositionGains(positionGains gains);
positionGains getPositionGains();

#endif