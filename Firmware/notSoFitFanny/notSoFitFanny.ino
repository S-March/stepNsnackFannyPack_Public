/******************
 * notSoFitFanny *
 * by Sam March   *
 *****************/
#include <Wire.h>
#include "LIS2DW12.h"

#define USER_BUTTON 0
#define MOTOR_SWITCH_PIN 9
#define LED_PIN 18
#define LEVEL_SHIFTER_ENABLE_PIN 19
#define XL_INT_1_PIN 13
#define XL_INT_2_PIN 14
#define MOTOR_ENABLE_PIN 33

#define DEBOUNCE_TIME 500
#define XL_SAMPLE_TIME 25
#define XL_NUMBER_OF_SAMPLES 64
#define XL_AVERAGE_SAMPLE_SIZE 512
#define STEP_SAMPLE_LOCKOUT 25
#define MOTOR_FORWARD_LOCKOUT 100
#define STEPS_NEEDED_FOR_DISPENSE 50

int buttonPressTime;
int xlInterruptTime;
int motorForwardTime;
int updateXLTime;
int newTime;

int xl_data_X[XL_NUMBER_OF_SAMPLES];
int xl_data_Y[XL_NUMBER_OF_SAMPLES];
int xl_data_Z[XL_NUMBER_OF_SAMPLES];

int xl_long_average[XL_AVERAGE_SAMPLE_SIZE];
int xlLongAverage;
int longAverageCounter;
bool stepTaken;
bool stepTakeLockedOut;
int stepSampleLockoutCounter;
int numberOfStepsTaken;

int xlDataOutputX;
int xlDataOutputY;
int xlDataOutputZ;

int xlDataLastOutputX;
int xlDataLastOutputY;
int xlDataLastOutputZ;

int xlDataCounter;
int deltaAVGCounter;

int currentStepData;
int lastStepData;

bool motorStopping;
bool buttonDebouncing;
bool motorActive;
bool xlInterrupt1Triggered;
bool xlInterruptDebouncing;

struct buttonStruct 
{
  const uint8_t PIN;
  bool buttonPressed;
};

buttonStruct motorSwitch = {MOTOR_SWITCH_PIN, false};
buttonStruct userButtonSwitch = {USER_BUTTON, false};

void IRAM_ATTR userButtonSwitchISR() 
{
  userButtonSwitch.buttonPressed = true;
}

void IRAM_ATTR motorSwitchISR() 
{
  motorSwitch.buttonPressed = true;
}

void IRAM_ATTR xlInt1ISR()
{
  xlInterrupt1Triggered = true;
}

void motorControl(bool STATE);
void stepCounterHandler(void);

void setup() { 
	Serial.begin(115200);
  Serial.println("Booting!");

  Wire.begin();
  Serial.println("Looking for accelerometer");
  if(xl_whoAmI())
  {
    Serial.println("XL found");
    xl_initialize();
  }
  else
  {
    Serial.println("XL not found");
  }

  motorStopping = false;
  buttonDebouncing = false;
  motorActive = false;
  xlInterrupt1Triggered = true;//false;
  xlInterruptDebouncing = false;

  xlDataCounter = 0;
  deltaAVGCounter = 0;
  longAverageCounter = 0;
  
  currentStepData = 0;
  lastStepData = 0;
  stepTaken = false;
  stepTakeLockedOut = false;
  stepSampleLockoutCounter = 0;
  numberOfStepsTaken = 0;

  pinMode(MOTOR_SWITCH_PIN, INPUT);
  pinMode(USER_BUTTON, INPUT_PULLUP);
  pinMode(XL_INT_1_PIN, INPUT);
  pinMode(XL_INT_2_PIN, INPUT);
  pinMode(LEVEL_SHIFTER_ENABLE_PIN, OUTPUT);
  pinMode(MOTOR_ENABLE_PIN, OUTPUT);
  
  digitalWrite(LEVEL_SHIFTER_ENABLE_PIN, HIGH);
  
  attachInterrupt(XL_INT_1_PIN, xlInt1ISR, FALLING);
  attachInterrupt(MOTOR_SWITCH_PIN, motorSwitchISR, FALLING);
  attachInterrupt(USER_BUTTON, userButtonSwitchISR, FALLING);
}

void loop() { 
  newTime = millis();
  stepCounterHandler();
  if (userButtonSwitch.buttonPressed)
  {
      detachInterrupt(userButtonSwitch.PIN); 
      userButtonSwitch.buttonPressed = false;
      buttonDebouncing = true;
      buttonPressTime = newTime;
      motorControl(true);
  }
  if (motorSwitch.buttonPressed)
  {
      detachInterrupt(motorSwitch.PIN);
      motorSwitch.buttonPressed = false;
      motorStopping = true;
      buttonDebouncing = true;
      motorForwardTime = newTime;
      buttonPressTime = newTime;
  }
  if(xlInterruptDebouncing&&(newTime-xlInterruptTime>DEBOUNCE_TIME))
  {    
    xlInterruptDebouncing = false;
    attachInterrupt(XL_INT_1_PIN, xlInt1ISR, RISING); 
  }
  if(motorStopping&&(newTime-motorForwardTime>MOTOR_FORWARD_LOCKOUT))
  {
    motorStopping = false;
    motorControl(false);
  }
  if(buttonDebouncing&&(newTime-buttonPressTime>DEBOUNCE_TIME))
  {    
    buttonDebouncing = false;
    attachInterrupt(motorSwitch.PIN, motorSwitchISR, FALLING);
    attachInterrupt(userButtonSwitch.PIN, userButtonSwitchISR, FALLING);    
  }
}

void stepCounterHandler(void)
{
  if(newTime-updateXLTime>XL_SAMPLE_TIME)
  {
    if((stepTakeLockedOut)&&(stepSampleLockoutCounter < STEP_SAMPLE_LOCKOUT))
    {
      stepSampleLockoutCounter++;
    }
    else
    {
      stepSampleLockoutCounter = 0;
      stepTakeLockedOut = false;
    }
    if(xlDataCounter<XL_NUMBER_OF_SAMPLES)
    {
      xl_data_X[xlDataCounter] = xl_readXAxis();
      xl_data_Y[xlDataCounter] = xl_readYAxis();
      xl_data_Z[xlDataCounter] = xl_readZAxis();
      xlDataCounter++;
    }
    else
    {
      xlDataCounter = 0;
      xl_data_X[xlDataCounter] = xl_readXAxis();
      xl_data_Y[xlDataCounter] = xl_readYAxis();
      xl_data_Z[xlDataCounter] = xl_readZAxis();
    }
    xlDataOutputX = 0;
    xlDataOutputY = 0;
    xlDataOutputZ = 0;
    for(int i=0;i<XL_NUMBER_OF_SAMPLES;i++)
    {
      xlDataOutputX += xl_data_X[i];
      xlDataOutputY += xl_data_Y[i];
      xlDataOutputZ += xl_data_Z[i];
    }
    xlDataOutputX /= XL_NUMBER_OF_SAMPLES;
    xlDataOutputY /= XL_NUMBER_OF_SAMPLES;
    xlDataOutputZ /= XL_NUMBER_OF_SAMPLES; 

    xlDataLastOutputX = (xlDataOutputY-xlDataOutputX);
    xlDataLastOutputY = (xlDataOutputZ-xlDataOutputY);
    xlDataLastOutputZ = (xlDataOutputX-xlDataOutputZ);

    if(longAverageCounter < XL_AVERAGE_SAMPLE_SIZE)
    {
      xl_long_average[longAverageCounter] = ((xlDataOutputZ-xlDataOutputY)-(xlDataOutputY-xlDataOutputX));
      longAverageCounter++;
    }
    else
    {
      longAverageCounter = 0;
      xl_long_average[longAverageCounter] = ((xlDataOutputZ-xlDataOutputY)-(xlDataOutputY-xlDataOutputX));
    }
    for(int i=0;i<XL_AVERAGE_SAMPLE_SIZE;i++)
    {
      xlLongAverage+=xl_long_average[i];
    }
    xlLongAverage/=XL_AVERAGE_SAMPLE_SIZE;

    currentStepData = abs(xlDataLastOutputX-xlDataLastOutputY)-abs(xlLongAverage);
    if(!stepTakeLockedOut&&(currentStepData<0)&&(lastStepData>0))
    {
      stepTaken = true;
      stepTakeLockedOut = true;
    }
    lastStepData=currentStepData;
      
//    Serial.print("(y-x)-(z-y):");
//    Serial.print(abs(xlDataLastOutputX-xlDataLastOutputY));
//    Serial.print(",long:");
//    Serial.print(abs(xlLongAverage));
//    Serial.print(",(z-y)-(x-z):");
    Serial.print("XL_Data:");
    Serial.print(abs(xlDataLastOutputX-xlDataLastOutputY)-abs(xlLongAverage));
    if(stepTaken)
    {
      stepTaken = false;
      numberOfStepsTaken++;
      if((numberOfStepsTaken%STEPS_NEEDED_FOR_DISPENSE)==0)
      {
        userButtonSwitch.buttonPressed = true;
        numberOfStepsTaken = 0;
      }
      Serial.print(",stepTaken:");
      Serial.print(10000);
    }
    else
    {
      Serial.print(",stepTaken:");
      Serial.print(0);
    }
    if(stepTakeLockedOut)
    {
      Serial.print(",stepLockedOut:");
      Serial.print(5000);
    }
    else
    {
      Serial.print(",stepLockedOut:");
      Serial.print(0);
    }
    Serial.println("");
    updateXLTime = newTime;
  }
}

void motorControl(bool STATE)
{
//  if(STATE)
//  {    
//    Serial.println("Motor ON");
//  }
//  else
//  {
//    Serial.println("Motor OFF");
//  }
  digitalWrite(MOTOR_ENABLE_PIN, STATE);
}
