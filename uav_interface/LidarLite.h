#ifndef LIDARLITE_H_
#define LIDARLITE_H_

#include "Device.h"



class LidarLite : public Device
{
  public:
    LidarLite(char *name, uint8_t i2cAddr);
    void Update();

  private:
    void write(char addr, char val);
    void read(char addr, int numBytes, uint8_t array[]);
  
    uint8_t _i2cAddr;
    int16_t _distance;
};

#endif
