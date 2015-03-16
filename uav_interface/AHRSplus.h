#ifndef AHRSPLUS_H_
#define AHRSPLUS_H_

#include "Device.h"

class AHRSplus : public Device
{
  public:
    AHRSplus(char* name);
    void Update();

  private:
    uint16_t _accX;
    uint16_t _accY;
    uint16_t _accZ;
    uint16_t _gyroX;
    uint16_t _gyroY;
    uint16_t _gyroZ;
    uint16_t _magX;
    uint16_t _magY;
    uint16_t _magZ;
    uint16_t _temp;
    uint16_t _roll;
    uint16_t _pitch;
    uint16_t _yaw;
    uint16_t _quaternionX;
    uint16_t _quaternionY;
    uint16_t _quaternionZ;
    uint16_t _quaternionW;
	
    uint16_t registerRead(uint8_t registerAddress);
};

#endif
