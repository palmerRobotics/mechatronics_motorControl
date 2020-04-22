#include "currentControl.h"
#include "utilities.h"
#include "iSense.h"
#include "xc.h"
#include <sys/attribs.h> 
#include "stdio.h"
#include "fir.h"
#include "NU32.h"

#define PERIOD_5KHZ 15999
#define PERIOD_20KHZ 3999 //1999
#define ITEST_LENGTH 100
#define PERCENT_TO_DECIMAL 100
#define KP_SCALING 100
#define KI_SCALING 100
#define FORWARD 0
#define REVERSE 1

static char buffer[200];

static currentGains_t iGains;
static FIRfilter LPfilter;
//static int order = 4;
//static float b[5] = {1, 0, 0, 0, 0};
static int order = 2;
static float b[3] = {.0665, .8670, .0665};

static volatile int dutyCount = 0;
static volatile direction;
static volatile float iTestLog[ITEST_LENGTH]; //need to determine how many samples will occur in itest
static volatile float iTestRef[ITEST_LENGTH];
static volatile float iTestU[ITEST_LENGTH];
static volatile float pwm[ITEST_LENGTH];

void __ISR(_TIMER_3_VECTOR, IPL6SRS) currentISR(void){
  static float iRef;
  static float iActual = 0;
	static float iError = 0;

	static float iErrorInt = 0;

		switch(getMode()){
			case IDLE:
			{
				//NU32_LED1 = 1;//FOR DEBUGGING
				//NU32_LED2 = 0;
				OC1RS = 0;
				dutyCount = 0;
        resetFilter(order, &LPfilter);
				break;
			}
			case PWM:
			{
				//NU32_LED1 = 0;
				//NU32_LED2 = 1;
				OC1RS = dutyCount;
				if(direction == FORWARD){
					LATDbits.LATD1 = FORWARD;
				}
				else{
					LATDbits.LATD1 = REVERSE; //set direction of pwm
				}
				break;
			}
			case ITEST:
			{
				static int count = 0;
				if(count == 0){
					iRef = 200;
				}
        else if(count == ITEST_LENGTH){
					setMode(IDLE);
          count = 0;
					sendCurrentData();  //!!!may need to do this one by one, not all at once 
          break; //is this ok/good coding standard to do?
				}
				else if(count % 25 == 0 && count != 0){
					iRef = iRef * -1; //invert iRef
          //sprintf(buffer,"\n"); //FOR DEBUGGING
          //NU32_WriteUART3(buffer);
				}
				//iActual = getCurrentmAmps();
        iActual = filterSignal(getCurrentmAmps(), &LPfilter);

        /*if(iActual < -400){
          iActual = -400;
        }*/

				iError = iRef - iActual;

        iErrorInt = iErrorInt + iError;
        if(iErrorInt > 400){
          iErrorInt = 400;
        }

        //dutyCount = iGains.kp*iError;
        dutyCount = iGains.kp*iError + iGains.ki*iErrorInt;

        if(dutyCount >= 0){
          LATDbits.LATD1 = FORWARD;
        }
        else{
          LATDbits.LATD1 = REVERSE;
          dutyCount = -1*dutyCount; //direction has been set, and absolute value of dutyCount is needed
        }

        if(dutyCount > PERIOD_20KHZ){
          dutyCount = PERIOD_20KHZ;
        }

        OC1RS = dutyCount;

        

        pwm[count] = (float)dutyCount/(PERIOD_20KHZ + 1); //FOR DEBUGGING

				iTestRef[count] = iRef;
				iTestLog[count] = iActual;
        if(LATDbits.LATD1 == FORWARD){
          iTestU[count] = (float)OC1RS/40;
        }
        else{
          iTestU[count] = (float)-1*OC1RS/40;
        }
        
        //sprintf(buffer, "R: %4.0f | A: %4.0f | E: %4.0f | OC1RS: %5d | Dir: %d | PWM: %6.3f\r\n", iRef, iActual, iError, OC1RS, LATDbits.LATD1, iTestU[count]);
        //NU32_WriteUART3(buffer);
				
				//NU32_LED1 = 0; //for debugging
				//NU32_LED2 = 0;
				count++;
				break;
			}
			case HOLD:
			{
				break;
			}
			case TRACK:
			{
				break;
			}
			default:
			{
				break;
			}
		}
	IFS0bits.T3IF = 0; //clear interrupt flag
}

void initIcontrol(){
	direction = FORWARD;
	__builtin_disable_interrupts();
	//initialize and enable current control timer
	PR3 = PERIOD_5KHZ;
	TMR3 = 0;
	T3CONbits.TCKPS = 0; //set prescaler
	T3CONbits.TGATE = 0;
	T3CONbits.ON = 1;  //enable timer3
	IPC3bits.T3IP = 6; //priority level 6
	IPC3bits.T3IS = 0; //subpriority 0
	IFS0bits.T3IF = 0; //clear interrupt flag
	IEC0bits.T3IE = 1; //enable interrupt
	__builtin_enable_interrupts();
	

	//initialize and enable PWM timer
	PR2 = PERIOD_20KHZ;
	TMR2 = 0;
	T2CONbits.TCKPS = 0; //0b010
	OC1CONbits.OCM = 0b110;
	OC1RS = 0;
	OC1R = 0;
	T2CONbits.ON = 1;
	OC1CONbits.ON = 1;
	
	TRISDbits.TRISD1 = 0; //set D1 as a digital out pin

	iGains.kp = 0;
	iGains.ki = 0;
  initFilter(order, b, &LPfilter);
	setMode(IDLE);
}

void setPWM(int dutyCycle){
	if(dutyCycle >= 0){
		direction = FORWARD;
		dutyCount = ((PERIOD_20KHZ + 1)*dutyCycle)/PERCENT_TO_DECIMAL;
	}
	else{
		direction = REVERSE;
		dutyCount = ((PERIOD_20KHZ + 1)*(-1*dutyCycle))/PERCENT_TO_DECIMAL; //we want the absolute value of dutyCount, so times dutyCycle by -1
	}
	if(dutyCount > PERIOD_20KHZ){
		dutyCount = PERIOD_20KHZ;
	}
}

void setCurrent(){
	//set iRef to be tracked by the current controller
}

void setCurrentGains(currentGains_t gains){
	//scale kp and ki here
	iGains = gains;
}
currentGains_t getCurrentGains(){
	//unscale kp and ki before returning them
	return iGains;
}

void sendCurrentData(){
	int i;
  //sprintf(buffer, "Printing Logged Data:\r\n");
	//NU32_WriteUART3(buffer);
  sprintf(buffer, "%d\r\n", ITEST_LENGTH);
  NU32_WriteUART3(buffer);
	for(i = 0; i < ITEST_LENGTH; i++){
    sprintf(buffer, "%f %f %f\r\n", iTestRef[i], iTestLog[i], iTestU[i]);
    NU32_WriteUART3(buffer);
		//sprintf(buffer, "%2d  Ref: %5.1f  Act: %5.1f  DC: 43d\r\n", i, iTestRef[i], iTestLog[i], DC[i]);
		//NU32_WriteUART3(buffer);
	}
	
}