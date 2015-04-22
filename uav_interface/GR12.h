#ifndef GR12_H_
#define GR12_H_

#include "Arduino.h"

#define HANDLE_CHANGE_PORT(PORT_NR)                                        \
volatile uint64_t lastPulseTime##PORT_NR;                                  \
volatile uint64_t lastPulseDuration##PORT_NR;                              \
                                                                           \
void handleChangePort##PORT_NR ()                                          \
{                                                                          \
  if((micros() - lastPulseTime##PORT_NR ) > 10000)                         \
  {                                                                        \
    lastPulseTime##PORT_NR = micros();                                     \
  }                                                                        \
  else                                                                     \
  {                                                                        \
    lastPulseDuration##PORT_NR = micros() - lastPulseTime##PORT_NR;        \
    lastPulseTime##PORT_NR = micros();                                     \
  }                                                                        \
}  

#include "Device.h"

class GR12 : public Device
{
  public:
    GR12(char* name);
    void Update();

  private:
    double _pitch;
    double _yaw;
    double _gaz;
    double _roll;
    uint8_t _isAutonomous;
   
    double dutyCycleToStickValue(uint64_t pulseTime, double minPulseTime, double maxPulseTime); 
    virtual void OnRequest();
};

#endif
