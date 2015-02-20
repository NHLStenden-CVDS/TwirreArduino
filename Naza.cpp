#include "Naza.h"
#include "Arduino.h"

//#include "Adafruit_PWMServoDriver.h"
//225, 900
#define PWM_MIN 2250
//389, 1556
#define PWM_MAX 3890
//307, 1228
#define PWM_MIDDLE 3070

#define PITCH_CHANNEL 6
#define ROLL_CHANNEL 7
#define YAW_CHANNEL 4
#define GAZ_CHANNEL 5
#define CONTROL_CHANNEL 8

Naza::Naza(char* name) : Device(name, "With this actuator you can control Naza flight controllers")
{
  analogWriteResolution(12);
  
  //we add the variable so it can be changed
  _AddVariable("pitch", &_pitch);
  _AddVariable("roll", &_roll);
  _AddVariable("yaw", &_yaw);
  _AddVariable("gaz", &_gaz);
  _AddVariable("timeout", &_timeout);
  
  analogWrite(PITCH_CHANNEL, PWM_MIDDLE);
  analogWrite(ROLL_CHANNEL, PWM_MIDDLE);
  analogWrite(YAW_CHANNEL, PWM_MIDDLE);
  analogWrite(GAZ_CHANNEL, PWM_MIDDLE);
}

void Naza::ValuesChanged()
{
  uint16_t pulselengthPitch = map(_pitch, -1, 1, PWM_MIN, PWM_MAX);
  uint16_t pulselengthRoll = map(_roll, -1, 1, PWM_MIN, PWM_MAX);
  uint16_t pulselengthYaw = map(_yaw, -1, 1, PWM_MIN, PWM_MAX);
  uint16_t pulselengthGaz = map(_gaz, -1, 1, PWM_MIN, PWM_MAX);

  analogWrite(PITCH_CHANNEL, pulselengthPitch);
  analogWrite(ROLL_CHANNEL, pulselengthRoll);
  analogWrite(YAW_CHANNEL, pulselengthYaw);
  analogWrite(GAZ_CHANNEL, pulselengthGaz);
}

// TODO: Add timeout to interupts
int out = PWM_MIN;
int dir = 1;
void Naza::Update()
{
  out += dir;
  if(out > PWM_MAX)
  {
    out = PWM_MAX;
    dir = -1; 
  }
  if(out < PWM_MIN)
  {
    out = PWM_MIN;
    dir = 1; 
  }
  
  analogWrite(PITCH_CHANNEL, out);
  analogWrite(ROLL_CHANNEL, out);
  analogWrite(YAW_CHANNEL, out);
  analogWrite(GAZ_CHANNEL, out);
}
