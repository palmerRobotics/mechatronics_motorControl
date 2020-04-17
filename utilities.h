#ifndef UTILITIES__H__
#define UTILITIES__H__

#include "stdio.h"
#define MAX_SAMPLES 2000

typedef enum {IDLE, PWM, ITEST, HOLD, TRACK} opMode_t ;
static desiredTrajectory[MAX_SAMPLES] = {};

void setMode(int desiredMode);
int getMode();
void loadTrajectory(float trajectory[], int n);
#endif