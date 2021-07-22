#include <Arduino.h>
#include <Wire.h>
#include "LIS2DW12.h"

bool xl_whoAmI(void)
{
  bool returnValue = false;
  uint8_t whoAmIRead;
  Wire.beginTransmission(LIS2DW12_ADDRESS);
  Wire.write(LIS2DW12_WHO_AM_I);
  if(Wire.endTransmission() != 0)
  {
    Serial.println("Error reading from device");
  }
  Wire.requestFrom(LIS2DW12_ADDRESS, 1);
  while (Wire.available())
  {
    whoAmIRead = Wire.read(); // receive a byte as a proper uint8_t
  }  
  if(whoAmIRead==0x44)
  {
    returnValue = true;
  }
  return returnValue;
}
void xl_initialize(void)
{
  //Sampling rate and power mode
  Serial.println("Sampling rate and power mode set to 100Hz");
  Wire.beginTransmission(LIS2DW12_ADDRESS);
  Wire.write(LIS2DW12_CTRL1);
  Wire.write(0x74); //100hz sampling
  if(Wire.endTransmission() != 0)
  {
    Serial.println("Error reading from device");
  }
  
  //Filtering
  Serial.println("HP filter and low noise enabled");
  Wire.beginTransmission(LIS2DW12_ADDRESS);
  Wire.write(LIS2DW12_CTRL6);
  Wire.write(0x04); //HP filter and Low noise enabled
  if(Wire.endTransmission() != 0)
  {
    Serial.println("Error reading from device");
  }
  
  //Tap direction priority
  Wire.beginTransmission(LIS2DW12_ADDRESS);
  Wire.write(LIS2DW12_TAP_THS_X);
  Wire.write(0x09);
  if(Wire.endTransmission() != 0)
  {
    Serial.println("Error reading from device");
  }
  
  //Tap direction priority
  Wire.beginTransmission(LIS2DW12_ADDRESS);
  Wire.write(LIS2DW12_TAP_THS_Y);
  Wire.write(0xE9); //Z->Y->X
  if(Wire.endTransmission() != 0)
  {
    Serial.println("Error reading from device");
  }
  
  //Tap enable
  Wire.beginTransmission(LIS2DW12_ADDRESS);
  Wire.write(LIS2DW12_TAP_THS_Z);
  Wire.write(0xE9); //All axis enabled
  if(Wire.endTransmission() != 0)
  {
    Serial.println("Error reading from device");
  }
  
  //Tap duration
  Wire.beginTransmission(LIS2DW12_ADDRESS);
  Wire.write(LIS2DW12_INT_DUR);
  Wire.write(0x06); //All axis enabled
  if(Wire.endTransmission() != 0)
  {
    Serial.println("Error reading from device");
  }
  
  //Tap duration
  Wire.beginTransmission(LIS2DW12_ADDRESS);
  Wire.write(LIS2DW12_WAKE_UP_THS);
  Wire.write(0x00); //All axis enabled
  if(Wire.endTransmission() != 0)
  {
    Serial.println("Error reading from device");
  }
    
  //Interrupt pin source
  Serial.println("Interrupt actions routed to interrupt pin 1");
  Wire.beginTransmission(LIS2DW12_ADDRESS);
  Wire.write(LIS2DW12_CTRL4_INT1_PAD_CTRL);
  Wire.write(0x40); //All actions routed to interrupt pin 1
  if(Wire.endTransmission() != 0)
  {
    Serial.println("Error reading from device");
  }
  
  //Interrupt polarity
  Serial.println("Interrupt polarity set to active low with open drain");
  Wire.beginTransmission(LIS2DW12_ADDRESS);
  Wire.write(LIS2DW12_CTRL3);
  Wire.write(0x08); //Interrupt active high
  if(Wire.endTransmission() != 0)
  {
    Serial.println("Error reading from device");
  }

  //Enable interrupts
  Serial.println("Enable interrupts");
  Wire.beginTransmission(LIS2DW12_ADDRESS);
  Wire.write(LIS2DW12_CTRL_REG7);
  Wire.write(0x20); //Interrupts on
  if(Wire.endTransmission() != 0)
  {
    Serial.println("Error reading from device");
  } 
}

int16_t xl_readXAxis(void) {
    uint8_t msb = 0;
    uint8_t lsb = 0;

    Wire.beginTransmission(LIS2DW12_ADDRESS);
    Wire.write(LIS2DW12_OUT_X_H);  
    Wire.endTransmission(); 
    Wire.requestFrom(LIS2DW12_ADDRESS, (uint8_t)1);
    msb = Wire.read(); 
    
    Wire.beginTransmission(LIS2DW12_ADDRESS);
    Wire.write(LIS2DW12_OUT_X_L);  
    Wire.endTransmission(); 
    Wire.requestFrom(LIS2DW12_ADDRESS, (uint8_t)1);
    lsb = Wire.read(); 

    return (((int16_t)msb) << 8) | lsb;
}

int16_t xl_readYAxis(void) {
    uint8_t msb = 0;
    uint8_t lsb = 0;

    Wire.beginTransmission(LIS2DW12_ADDRESS);
    Wire.write(LIS2DW12_OUT_Y_H);  
    Wire.endTransmission(); 
    Wire.requestFrom(LIS2DW12_ADDRESS, (uint8_t)1);
    msb = Wire.read(); 
    
    Wire.beginTransmission(LIS2DW12_ADDRESS);
    Wire.write(LIS2DW12_OUT_Y_L);  
    Wire.endTransmission(); 
    Wire.requestFrom(LIS2DW12_ADDRESS, (uint8_t)1);
    lsb = Wire.read(); 

    return (((int16_t)msb) << 8) | lsb;
}

int16_t xl_readZAxis(void) {
    uint8_t msb = 0;
    uint8_t lsb = 0;

    Wire.beginTransmission(LIS2DW12_ADDRESS);
    Wire.write(LIS2DW12_OUT_Z_H);  
    Wire.endTransmission(); 
    Wire.requestFrom(LIS2DW12_ADDRESS, (uint8_t)1);
    msb = Wire.read(); 
    
    Wire.beginTransmission(LIS2DW12_ADDRESS);
    Wire.write(LIS2DW12_OUT_Z_L);  
    Wire.endTransmission(); 
    Wire.requestFrom(LIS2DW12_ADDRESS, (uint8_t)1);
    lsb = Wire.read(); 

    return (((int16_t)msb) << 8) | lsb;
}
