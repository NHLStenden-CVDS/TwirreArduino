#ifndef NAZA_H_
#define NAZA_H_

#include "Device.h"
#include "Adafruit_PWMServoDriver.h"

class Naza : public Device
{
  public:
    Naza(char* name, uint8_t servoDriverAddress = 0x40);
    void Update();
    void ValuesChanged();

  private:
    float _pitch;
    float _yaw;
    float _roll;
    float _gaz;
    uint32_t _timeout;
    Adafruit_PWMServoDriver _PWMDriver;

};

#endif
