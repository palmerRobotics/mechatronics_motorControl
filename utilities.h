#ifndef UTILITIES__H__
#define UTILITIES__H__

#include "stdio.h"
#define MAX_SAMPLES 2000

typedef enum {Idle, PWM, iTest, Hold, Track} opMode_t ;
static desiredTrajectory[MAX_SAMPLES] = {};

void setMode(opMode_t* modep, int desiredMode);
int getMode(opMode_t* modep);///I don't understand the point of this function...
void loadTrajectory(float trajectory[], int n);
#endif