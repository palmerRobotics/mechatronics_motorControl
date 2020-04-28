#include "currentControl.h"
#include "utilities.h"
#include "iSense.h"
#include "stdio.h"
#include "fir.h"
#include "NU32.h"

#define PERIOD_5KHZ 15999
#define PERIOD_20KHZ 3999 //1999
#define ITEST_LENGTH 100
#define PERCENT_TO_DECIMAL 100
#define ONE_HUNDRED_PERCENT 100
#define FORWARD 0
#define REVERSE 1

#define PWMTEST_LENGTH 1000

static char buffer[200];

static currentGains_t iGains;
static FIRfilter LPfilter;
//static int order = 2;
//static float b[3] = {1, 0, 0};
static int order = 2;
static float b[3] = {.0665, .8670, .0665};

static volatile int dutyCount = 0;
static volatile direction;
static volatile float iRef;
static volatile float iTestLog[ITEST_LENGTH];
static volatile float iTestRef[ITEST_LENGTH];
static volatile float iTestU[ITEST_LENGTH];
static volatile float iTestIerrorInt[ITEST_LENGTH];
static volatile float iTestIerror[ITEST_LENGTH];
static volatile float iPWM[PWMTEST_LENGTH];
static volatile float pwmlog[PWMTEST_LENGTH];

void __ISR(_TIMER_3_VECTOR, IPL6SRS) currentISR(void){
  static float iActual = 0;
  static float iActualPrev = 0;
	static float iError = 0;
	static float iErrorInt = 0;

		switch(getMode()){
			case IDLE:
			{
				OC1RS = 0;
				dutyCount = 0;
        resetFilter(order, &LPfilter);
				break;
			}
			case PWM:
			{
				OC1RS = dutyCount;
				if(direction == FORWARD){
					LATDbits.LATD1 = FORWARD; //set direction of pwm
				}
				else{
					LATDbits.LATD1 = REVERSE;
				}
        //sprintf(buffer, "%f\r\n", getCurrentmAmps());
        //NU32_WriteUART3(buffer);
				break;
			}
			case ITEST:
			{
				static int count = 0;
        static float u;
				if(count == 0){
					iRef = 200;
          dutyCount = 0;
				}
        else if(count == ITEST_LENGTH){
					setMode(IDLE);
          count = 0;
          iErrorInt = 0;
					sendCurrentData();
          break;
				}
				else if(count % 25 == 0 && count != 0){
					iRef = iRef * -1; //invert iRef
          //iRef = iRef - 50;
          //iRef = 0;
          //sprintf(buffer,"\n"); //FOR DEBUGGING
          //NU32_WriteUART3(buffer);
				}
        iActual = filterSignal(getCurrentmAmps(), &LPfilter);
        //if(iActualPrev - iActual){}
				iError = iRef - iActual;
        iErrorInt = iErrorInt + iError;
        iTestIerror[count] = iError;
        if(iErrorInt > ONE_HUNDRED_PERCENT){
          iErrorInt = ONE_HUNDRED_PERCENT;
        }
        else if(iErrorInt < -ONE_HUNDRED_PERCENT){
          iErrorInt = -ONE_HUNDRED_PERCENT;
        }
        iTestIerrorInt[count] = iErrorInt;

        u = iGains.kp*iError + iGains.ki*iErrorInt;
        if(u >= 0){
          LATDbits.LATD1 = FORWARD;
        }
        else{
          LATDbits.LATD1 = REVERSE;
          u = u*-1;//invert u if u is negative
        }
        if(u > 100){
          u = 100; //saturate u at 100%
        }
        u = u /ONE_HUNDRED_PERCENT; //calculate u as a percentage
        OC1RS = (unsigned int)(u*PERIOD_20KHZ);


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
				count++;
				break;
			}
			case HOLD:
			{
        static float u = 0;
        iActual = filterSignal(getCurrentmAmps(), &LPfilter);
				iError = iRef - iActual;
        iErrorInt = iErrorInt + iError;
        if(iErrorInt > ONE_HUNDRED_PERCENT){
          iErrorInt = ONE_HUNDRED_PERCENT;
        }
        else if(iErrorInt < -ONE_HUNDRED_PERCENT){
          iErrorInt = -ONE_HUNDRED_PERCENT;
        }

        u = iGains.kp*iError + iGains.ki*iErrorInt;
        u = u /ONE_HUNDRED_PERCENT; //calculate u as a percentage
        //sprintf(buffer, "R: %4.0f | A: %4.0f | u: %5.2f\r\n", iRef, iActual, u);
        //NU32_WriteUART3(buffer);
        if(u >= 0){
          LATDbits.LATD1 = FORWARD;
        }
        else{
          LATDbits.LATD1 = REVERSE;
          u = u*-1;//invert u if u is negative
        }
        if(u > ONE_HUNDRED_PERCENT){
          u = ONE_HUNDRED_PERCENT; //saturate u at 100%
        }
        OC1RS = (unsigned int)(u*PERIOD_20KHZ);
        break;
			}
			case TRACK:
			{
        static float u = 0;
        iActual = filterSignal(getCurrentmAmps(), &LPfilter);
				iError = iRef - iActual;
        iErrorInt = iErrorInt + iError;
        if(iErrorInt > ONE_HUNDRED_PERCENT){
          iErrorInt = ONE_HUNDRED_PERCENT;
        }
        else if(iErrorInt < -ONE_HUNDRED_PERCENT){
          iErrorInt = -ONE_HUNDRED_PERCENT;
        }

        u = iGains.kp*iError + iGains.ki*iErrorInt;
        u = u /ONE_HUNDRED_PERCENT; //calculate u as a percentage
        //sprintf(buffer, "R: %4.0f | A: %4.0f | u: %5.2f\r\n", iRef, iActual, u);
        //NU32_WriteUART3(buffer);
        if(u >= 0){
          LATDbits.LATD1 = FORWARD;
        }
        else{
          LATDbits.LATD1 = REVERSE;
          u = u*-1;//invert u if u is negative
        }
        if(u > ONE_HUNDRED_PERCENT){
          u = ONE_HUNDRED_PERCENT; //saturate u at 100%
        }
        OC1RS = (unsigned int)(u*PERIOD_20KHZ);
				break;
			}
      case PWMTEST:{
        static int count = 0;
        static float pwm = 0;
				if(count == 0){
          pwm = 0;
          if(pwm >= 0){
            LATDbits.LATD1 = FORWARD;
            OC1RS = ((PERIOD_20KHZ + 1)*pwm)/PERCENT_TO_DECIMAL;
          }
          else{
            LATDbits.LATD1 = REVERSE;
            OC1RS = -1*((PERIOD_20KHZ + 1)*pwm)/PERCENT_TO_DECIMAL;
          }
				}
        else if(count == PWMTEST_LENGTH){
          count = 0;
          setMode(IDLE);
          pwm = 0;
          OC1RS = pwm;
          sendPWMtestData();
          break;
				}
				else if(count % 1250 == 0 && count != 0){ //increment PWM% every 1250 samples
          pwm = pwm + 10;
          if(pwm >= 0){
            LATDbits.LATD1 = FORWARD;
            OC1RS = ((PERIOD_20KHZ + 1)*pwm)/PERCENT_TO_DECIMAL;
          }
          else{
            LATDbits.LATD1 = REVERSE;
            OC1RS = -1*((PERIOD_20KHZ + 1)*pwm)/PERCENT_TO_DECIMAL;
          }
          
				}
        pwmlog[count] = pwm;
        iPWM[count] = getCurrentmAmps();
        count++;
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
	T3CONbits.TCKPS = 0; //set prescaler to 1
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
  ODCDbits.ODCD1 = 0;

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

void setCurrent(float iDesired){
	//set iRef to be tracked by the current controller
  iRef = iDesired;
}

void setCurrentGains(currentGains_t gains){
	iGains = gains;
}
currentGains_t getCurrentGains(){
	return iGains;
}

void sendCurrentData(){
	int i;
  sprintf(buffer, "%d\r\n", ITEST_LENGTH);
  NU32_WriteUART3(buffer);
	for(i = 0; i < ITEST_LENGTH; i++){
    sprintf(buffer, "%f %f %f %f %f\r\n", iTestRef[i], iTestLog[i], iTestU[i], iTestIerror[i], iTestIerrorInt[i]);
    NU32_WriteUART3(buffer);
	}
}

void sendPWMtestData(){
  int i;
  sprintf(buffer, "%d\r\n", PWMTEST_LENGTH);
  NU32_WriteUART3(buffer);
  for(i = 0; i < PWMTEST_LENGTH; i++){
    sprintf(buffer, "%f %f\r\n", pwmlog[i], iPWM[i]);
    NU32_WriteUART3(buffer);
  }
}