#include "positionControl.h"
#include "currentControl.h"
#include "utilities.h"
#include "encoder.h"
#include "iSense.h"
#include "NU32.h"

#define TRACK_LENGTH 2000
static char buffer[200];

static positionGains_t posGains;
static volatile float thetaRef = 0;
static volatile float avgError = 0;
static volatile float thetaActs[TRACK_LENGTH];
static volatile float thetaRefs[TRACK_LENGTH];

void __ISR(_TIMER_4_VECTOR, IPL3SOFT) positionISR(void){
  static float thetaActual = 0;
  static float thetaErrorPrev = 0;
  static float thetaError = 0;
  static float thetaErrorInt = 0;
  static float thetaErrorDot = 0;
  static float iRef = 0;
  static float iAct = 0;

  switch(getMode()){
    case HOLD:
    {
      thetaActual = getEncoderDegrees();
      thetaError = thetaRef - thetaActual;
      avgError += thetaError;
      thetaErrorInt = thetaErrorInt + thetaError;
      thetaErrorDot = thetaError - thetaErrorPrev;
      if(thetaErrorInt > 10){
          thetaErrorInt = 10;
      }
      else if(thetaErrorInt < -10){
        thetaErrorInt = -10;
      }
      iRef = posGains.kp*thetaError + posGains.ki*thetaErrorInt + posGains.kd*thetaErrorDot; //iRef in units of mA
      setCurrent(iRef);
      iAct = getCurrentmAmps();
      //sprintf(buffer, "R: %6.2f | A: %5.1f | E: %5.1f | I: %5.1f | D: %5.1f | u: %6.2f | i: %6.2f\r\n", thetaRef, thetaActual, thetaError, thetaErrorInt, thetaErrorDot, iRef, iAct);
      //NU32_WriteUART3(buffer);
      thetaErrorPrev = thetaError;
      break;
    }
    case TRACK:
    {
      static int trajectoryLength = 0;
      static int count = 0;
      if(count == 0){
        trajectoryLength =  getTrajectoryLength();
      }
      if(count < trajectoryLength){
        thetaRef = getTrajectory(count);
        thetaRefs[count] = thetaRef;

        //sprintf(buffer, "%f\r\n", thetaRefs[count]);
        //NU32_WriteUART3(buffer);
        thetaActual = getEncoderDegrees();
        thetaActs[count] = thetaActual;
        thetaError = thetaRef - thetaActual;
        thetaErrorInt = thetaErrorInt + thetaError;
        thetaErrorDot = thetaError - thetaErrorPrev;
        //sprintf(buffer, "tRef: %6.2f | tAct: %6.2f | tE: %6.2f\r\n", thetaRefs[count], thetaActs[count], thetaError);
        //NU32_WriteUART3(buffer);
        if(thetaErrorInt > 10){
            thetaErrorInt = 10;
        }
        else if(thetaErrorInt < -10){
          thetaErrorInt = -10;
        }
        iRef = posGains.kp*thetaError + posGains.ki*thetaErrorInt + posGains.kd*thetaErrorDot; //iRef in units of mA
        setCurrent(iRef);
        iAct = getCurrentmAmps();
        //sprintf(buffer, "R: %6.2f | A: %5.1f | E: %5.1f | I: %5.1f | D: %5.1f | u: %6.2f | i: %6.2f\r\n", thetaRef, thetaActual, thetaError, thetaErrorInt, thetaErrorDot, iRef, iAct);
        //NU32_WriteUART3(buffer);
        thetaErrorPrev = thetaError;
        count++;
      }
      else{
        //send data back to matlab
        //sprintf(buffer, "Reached end of trajectory; setting IDLE\r\n");
        //NU32_WriteUART3(buffer);
        count = 0;
        sendTrajectory(); //!! COULD POTENTIALLY INTERRUPT ITSELF
        setMode(HOLD);
      }
      break;
    }
    default:
    {
      break;
    }
  }
  IFS0bits.T4IF = 0; //clear interrupt flag
}

void initPositionControl(){
    __builtin_disable_interrupts();
    //initialize and enable position control timer
    PR4 = 49999;
    TMR4 = 0;
    T4CONbits.TCKPS = 0b011; //set prescaler to 8
    T4CONbits.TGATE = 0;
    T4CONbits.ON = 1;  //enable timer4
    IPC4bits.T4IP = 3; //priority level 3
    IPC4bits.T4IS = 0; //subpriority 0
    IFS0bits.T4IF = 0; //clear interrupt flag
    IEC0bits.T4IE = 1; //enable interrupt
    __builtin_enable_interrupts();
}


void setPositionGains(positionGains_t gains){
  posGains = gains;
}

void setDesiredAngle(float angle){
  thetaRef = angle;
}

positionGains_t getPositionGains(){
    return posGains;
}

void sendTrajectory(){
  int i = 0;
  int n = getTrajectoryLength();
  sprintf(buffer, "%d\r\n", n);
  NU32_WriteUART3(buffer);
  for(i = 0; i < n; i++){
    sprintf(buffer, "%f %f\r\n", thetaRefs[i], thetaActs[i]);
    NU32_WriteUART3(buffer);
  }
  //sprintf(buffer, "%f\r\n", avgError/n);
  //NU32_WriteUART3(buffer);
}