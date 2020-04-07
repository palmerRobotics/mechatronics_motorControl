#ifndef POSITIONCONTROL__H__
#define POSOTIONCONTROL__H__

typedef struct{
    float kp;
    float ki;
    float kd;
} positionGains;

void init();
void loadTrajectory();
void setPositionGains();
positionGains getPositionGains();

#endif