#ifndef AHRSPLUS_H_
#define AHRSPLUS_H_

#include "Device.h"

#pragma pack(push, 1)
struct AHRSdata
{
  uint16_t accX;
    uint16_t accY;
    uint16_t accZ;
    uint16_t gyroX;
    uint16_t gyroY;
    uint16_t gyroZ;
    uint16_t magX;
    uint16_t magY;
    uint16_t magZ;
    uint16_t temp;
    uint16_t roll;
    uint16_t pitch;
    uint16_t yaw;
    uint16_t quaternionX;
    uint16_t quaternionY;
    uint16_t quaternionZ;
    uint16_t quaternionW;
};

struct AHRSdataNormalized
{
  float accX;
  float accY;
  float accZ;
  float gyroX;
  float gyroY;
  float gyroZ;
  float magX;
  float magY;
  float magZ;
  float temp;
  float roll;
  float pitch;
  float yaw;
  float quaternionX;
  float quaternionY;
  float quaternionZ;
  float quaternionW;
};

#pragma pack(pop)

class AHRSplus : public Device
{
  public:
    AHRSplus(char* name);
    void Update();

  private:
    AHRSdata _data;
    AHRSdataNormalized _normalizedData;
    unsigned long _time;
    uint16_t readBytes(uint8_t* dest, uint8_t regAddress, int count);
    void dataToNormalizedData();
};

#endif
