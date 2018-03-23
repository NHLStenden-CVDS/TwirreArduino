#ifndef BOAT_H_
#define BOAT_H_

#include "Device.h"
#include "Arduino.h"
#include <inttypes.h>

class Boat : public Device
{
  public:
    Boat(const char* name);
    void Update();
    
    float _DUTY_MAIN_THRUSTER = 0;
    float _DUTY_BOW_THRUSTER = 0;
    float _DUTY_RUDDER = 0;

    uint8_t _DIR_MAIN_THRUSTER = 1;
    uint8_t _DIR_BOW_THRUSTER = 1;
    uint8_t _DIR_RUDDER = 1;
    
  private:
    uint16_t lastUpdate; 
    int state = 0, prev_DIR_MAIN_THRUSTER = 0, prev_DIR_BOW_THRUSTER = 0;

    int InitialiseDutyCycle(int pin, int dutycycle);
    
    void SetDutyCycle_Main_Thruster(int duty_Main_Thruster, int dir_Main_Thruster);
    void SetDutyCycle_Bow_Thruster(int duty_Bow_Thruster, int dir_Bow_Thruster);
    void SetDutyCycle_Rudder(int duty_Rudder, int dir_Rudder);
};

#endif
