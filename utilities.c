#include "utilities.h"

#define BUF_SIZE 200

static opMode_t opMode;

void setMode(int desiredMode){
    opMode = desiredMode;
    return;
}

int getMode(){
    return opMode; //this seems weird
}

void loadTrajectory(float trajectory[], int n){//should trajectory be stored in utilities or main?
        int i;
        char buffer[BUF_SIZE];
        for(i = 0; i < n; i++){
            NU32_ReadUART3(buffer, BUF_SIZE);
            sscanf(buffer, "%f", &trajectory[i]);
        }
        return;
    }
