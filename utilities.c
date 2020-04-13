#include "utilities.h"

#define BUF_SIZE 200

void setMode(opMode_t* modep, int desiredMode){
    *modep = desiredMode;
    return;
}

int getMode(opMode_t* modep){
    return *modep; //this seems weird
}

void loadTrajectory(float trajectory[], int n){ //should I pass buf_size as an argument?
        int i;
        char buffer[BUF_SIZE];
        for(i = 0; i < n; i++){
            NU32_ReadUART3(buffer, BUF_SIZE);
            sscanf(buffer, "%f", &trajectory[i]);
        }
        return;
    }
