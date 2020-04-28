#ifndef UTILITIES__H__
#define UTILITIES__H__

#include "stdio.h"

typedef enum {IDLE, PWM, ITEST, HOLD, TRACK, PWMTEST} opMode_t ;

void setMode(int desiredMode);
int getMode();
void loadTrajectory();
float getTrajectory(int i);
int getTrajectoryLength();
#endif