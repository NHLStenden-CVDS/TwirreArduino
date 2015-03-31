#ifndef AHRSPLUS_H_
#define AHRSPLUS_H_

#include "Device.h"

#pragma pack(push, 1)
struct AHRSdata
{
  int16_t accX;
    int16_t accY;
    int16_t accZ;
    int16_t gyroX;
    int16_t gyroY;
    int16_t gyroZ;
    int16_t magX;
    int16_t magY;
    int16_t magZ;
    int16_t temp;
    int16_t roll;
    int16_t pitch;
    int16_t yaw;
    int16_t quaternionX;
    int16_t quaternionY;
    int16_t quaternionZ;
    int16_t quaternionW;
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
