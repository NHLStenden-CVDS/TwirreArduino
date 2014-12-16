#include "Naza.h"
#include "Adafruit_PWMServoDriver.h"

#define PWM_MIN 225
#define PWM_MAX 389
// TODO: set this value
#define CONTROL_VALUE 0

#define PITCH_CHANNEL 0
#define ROLL_CHANNEL 1
#define YAW_CHANNEL 2
#define GAZ_CHANNEL 3
#define CONTROL_CHANNEL 8

Naza::Naza(char* name, uint8_t servoDriverAddress) : Device(name, "With this actuator you can control Naza flight controllers")
{
  //we add the variable so it can be changed
  _AddVariable("pitch", &_pitch);
  _AddVariable("roll", &_roll);
  _AddVariable("yaw", &_yaw);
  _AddVariable("gaz", &_gaz);
  _AddVariable("timeout", &_timeout);
  
  _PWMDriver = Adafruit_PWMServoDriver(servoDriverAddress);
  _PWMDriver.begin();
  _PWMDriver.setPWMFreq(50);
  
}

void Naza::ValuesChanged()
{
  uint16_t pulselengthPitch = map(_pitch, -1, 1, PWM_MIN, PWM_MAX);
  uint16_t pulselengthRoll = map(_roll, -1, 1, PWM_MIN, PWM_MAX);
  uint16_t pulselengthYaw = map(_yaw, -1, 1, PWM_MIN, PWM_MAX);
  uint16_t pulselengthGaz = map(_gaz, -1, 1, PWM_MIN, PWM_MAX);

  _PWMDriver.setPWM(PITCH_CHANNEL, 0, pulselengthPitch);
  _PWMDriver.setPWM(ROLL_CHANNEL, 0, pulselengthRoll);
  _PWMDriver.setPWM(YAW_CHANNEL, 0, pulselengthYaw);
  _PWMDriver.setPWM(GAZ_CHANNEL, 0, pulselengthGaz);
}

// TODO: Add timeout to interupts
void Naza::Update()
{
  
}

