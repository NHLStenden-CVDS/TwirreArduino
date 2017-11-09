/*
 * Twirre: architecture for autonomous UAVs using interchangeable commodity components
 *
 * Copyright© 2017 Centre of expertise in Computer Vision & Data Science, NHL Stenden University of applied sciences
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

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

}

void LidarLite::OnRequest()
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


