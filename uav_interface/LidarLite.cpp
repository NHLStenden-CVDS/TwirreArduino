#include "LidarLite.h"
#include "Arduino.h"

#include <Wire.h>
#include <cstring>



LidarLite::LidarLite(char* name, uint8_t i2cAddr) : Device(name, "This is a Lidar Lite range finder")
{
  _i2cAddr = i2cAddr;
  _AddVariable("distance", &_distance);

  delay(10);
  write(0x00, 0x00);
  delay(10);
  write(0x45, 0x13);  //Continuous mode
                      //0x13 = 100Hz
                      //0xc8 =  10Hz
  delay(10);
  write(0x11, 0xff);  //No limit, keep on measuring
  delay(10);
  write(0x00, 0x04);  //Start continuous mode
}

void LidarLite::Update()
{
  uint8_t distance[2];
  read(0x8f,2,distance);
  _distance = (distance[0] << 8) + distance[1];
}

void LidarLite::write(char addr, char val)
{
  Wire.beginTransmission((int)_i2cAddr);
  Wire.write((int)addr);
  Wire.write((int)val);
  Wire.endTransmission();
}

void LidarLite::read(char addr, int numBytes, uint8_t result[])
{
  Wire.beginTransmission((int)_i2cAddr);
  Wire.write(addr);
  Wire.endTransmission();

  Wire.requestFrom((int)_i2cAddr, numBytes);

  int i = 0;
  if(numBytes <= Wire.available())
  {
    while(i < numBytes)
    {
      result[i] = Wire.read();
      i++;
    }
  } 
}


