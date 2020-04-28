#include "utilities.h"
#include "NU32.h"

#define BUF_SIZE 200
#define MAX_SAMPLES 2000

static opMode_t opMode;
static int trajectoryLength = 0;
static char buffer[BUF_SIZE];
static float trajectory[MAX_SAMPLES];
static float testtraj[MAX_SAMPLES] = {90,90,90,90,90,180,180,180,180, 180};


void setMode(int desiredMode){
    opMode = desiredMode;
    return;
}

int getMode(){
    return opMode;
}

void loadTrajectory(){
  int i = 0;
  char buffer[BUF_SIZE];
  NU32_ReadUART3(buffer, BUF_SIZE);
  sscanf(buffer, "%d", &trajectoryLength);  
  for(i = 0; i < trajectoryLength; i++){
    NU32_ReadUART3(buffer, BUF_SIZE);
    sscanf(buffer, "%f", &trajectory[i]);
  }
  for(i=0; i < trajectoryLength; i++){ //uncomment for debugging
    sprintf(buffer, "%f\r\n", trajectory[i]);
    NU32_WriteUART3(buffer);
  }
  return;
}

float getTrajectory(int i){
  return trajectory[i];
  //return testtraj[i];
}

int getTrajectoryLength(){
  return trajectoryLength;
}