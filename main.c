#include "NU32.h"          // config bits, constants, funcs for startup and UART
#include "stdio.h"
#include "math.h"
#include "iSense.h" // include other header files here
#include "currentControl.h"
#include "positionControl.h"
#include "encoder.h"
#include "utilities.h"
#include "fir.h"

#define BUF_SIZE 200
#define NUM_SAMPLES 1000

int main() 
{
  char buffer[BUF_SIZE];
  NU32_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
  NU32_LED1 = 1;  // turn off the LEDs
  NU32_LED2 = 1;
  __builtin_disable_interrupts();
  // in future, initialize modules or peripherals here
  __builtin_enable_interrupts();
  initEncoder();
  opMode_t opMode;
  while(1)
  {
    //sprintf(buffer, "Enter a command:\r\n"); //COMMENT WHEN USING MATLAB
    //NU32_WriteUART3(buffer);                 //COMMENT WHEN USING MATLAB
    NU32_ReadUART3(buffer,BUF_SIZE); // we expect the next character to be a menu command
    NU32_LED2 = 1;                   // clear the error LED
    switch (buffer[0]) {
      case 'a': //get ADC count
      {
        int count = getADCcount();
        sprintf(buffer,"%d\r\n",count);
        NU32_WriteUART3(buffer);
        break;
      }
      case 'b': //get current in mA
      {
        int mA = getCurrentmAmps();
        sprintf(buffer,"%d\r\n",mA);
        NU32_WriteUART3(buffer);
        break;
      }
      case 'c': //get encoder counts    COMPLETE
      {
        int n = getEncoderCount();
        sprintf(buffer,"%d\r\n",n);
        NU32_WriteUART3(buffer);
        break;
      }
      case 'd': //get encoder position in degrees   COMPLETE
      {
        int n = getEncoderDegrees(); //consider changing to a float
        sprintf(buffer,"%d\r\n",n);
        NU32_WriteUART3(buffer);
        break;
      }
      case 'e': //set current angle as zero position    COMPLETE
      {
        resetEncoder();
        break;
      }
      case 'f': //set PWM duty cycle
      {
        //function call to current control module: set fixed PWM
        break;
      }
      case 'g': //set current gains   COMPLETE
      {
        currentGains gains;
        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%f %f", &gains.kp, &gains.ki);
        setCurrentGains(gains);
        break;
      }
      case 'h': //get current gains   COMPLETE
      {
        currentGains gains = getCurrentGains();
        sprintf(buffer,"%f %f\r\n", gains.kp, gains.ki);
        NU32_WriteUART3(buffer);
        break;
      }
      case 'i': //set position gains    COMPLETE
      {
        positionGains gains;
        NU32_ReadUART3(buffer, BUF_SIZE);
        sscanf(buffer, "%f %f %f", &gains.kp, &gains.ki, &gains.kd);
        setPositionGains(gains);
        break;
      }
      case 'j': //get position gains    COMPLETE
      {
        positionGains gains = getPositionGains();
        sprintf(buffer,"%f %f %f\r\n", gains.kp, gains.ki, gains.kd);
        NU32_WriteUART3(buffer);
        break;
      }
      case 'k': //test current controller
      {
        //function call to current control module: test current controller
        break;
      }
      case 'l': //go to angle
      {
        //set operating mode
        //function call to position control module: go to angle
        break;
      }
      case 'm': //load step trajectory    COMPLETE
      {
        int i, n = 0;
        float trajectory[MAX_SAMPLES] = {};
        NU32_ReadUART3(buffer, BUF_SIZE);
        sscanf(buffer, "%d", &n);
        loadTrajectory(trajectory, n);
        /* //uncomment for debugging
        for(i=0; i < n; i++){
          sprintf(buffer, "%f\r\n", trajectory[i]);
          NU32_WriteUART3(buffer);
        }
        */
        break;
      }
      case 'n': //load cubic trajectory   COMPLETE
      {
        int i, n = 0;
        float trajectory[MAX_SAMPLES] = {};
        NU32_ReadUART3(buffer, BUF_SIZE);
        sscanf(buffer, "%d", &n);
        loadTrajectory(trajectory, n);
        /* //uncomment for debugging
        for(i=0; i < n; i++){
          sprintf(buffer, "%f\r\n", trajectory[i]);
          NU32_WriteUART3(buffer);
        }
        */
        break;
      }
      case 'o': //execute trajectory
      {
        //function call to position control module: execute trajectory
        break;
      }
      case 'p': //set PIC to IDLE mode    COMPLETE
      {
        setMode(&opMode, Idle);
        break;
      }
      case 'q': //quit    COMPLETE
      {
        setMode(&opMode, Idle);
        // handle q for quit. Later you may want to return to IDLE mode here. 
        break;
      }
      case 'r': //get operating mode
      {
        setMode(&opMode, PWM); //for debugging purposes. Will be set by other functions in final implementation
        int n = getMode(&opMode);
        sprintf(buffer, "current operating mode is: %d\r\n", n);
        NU32_WriteUART3(buffer);
        break;
      }
      case 's': //test FIR filter
      {
        //signal generation
        double Fs = 8192;
        float Fn = Fs/2;
        float wc = .2*Fn;
        float signal[NUM_SAMPLES] = {};
        float filterOut[NUM_SAMPLES] = {};
        int i;
        for(i=0; i < NUM_SAMPLES; i++){
            signal[i] = sin(2*M_PI*(.1*Fn)*(i/Fs)) + sin(2*M_PI*(.5*Fn)*(i/Fs));
        }

        FIRfilter LPfilter;
        int order = 9; //HARD CODE FILTER ORDER HERE (PREDETERMINED IN MATLAB)
        float b[MAX_ORDER + 1] = {-.0051,-.0133,.0155,.1630,.3399,
                                  .3399,.1630,.0155,-.0133,-.0051}; //HARD CODE FILTER COEFFICIENTS HERE (PRECALCULATED IN MATLAB)
        initFilter(order, b, &LPfilter);
        for(i=0; i < NUM_SAMPLES; i++){
            filterOut[i] = filterSignal(signal[i], &LPfilter);
        }
        sprintf(buffer, "%d\r\n", NUM_SAMPLES);
        NU32_WriteUART3(buffer);
        for(i = 0; i < NUM_SAMPLES; i++){
            sprintf(buffer, "%f %f\r\n", signal[i], filterOut[i]);
            NU32_WriteUART3(buffer);
        }
        break;
      }
      case 'x': // dummy command for demonstration purposes
      {
        int a, b = 0;
        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%d %d", &a, &b);
        sprintf(buffer,"%d\r\n", a+b); // return the number1 * number2
        NU32_WriteUART3(buffer);
        break;
      }
      case 'z': // dummy command for demonstration purposes
      {
        int n = 0;
        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%d", &n);
        sprintf(buffer,"%d\r\n", n + 1); // return the number + 1
        NU32_WriteUART3(buffer);
        break;
      }
      default:
      {
        NU32_LED2 = 0;  // turn on LED2 to indicate an error
        break;
      }
    }
  }
  return 0;
}
