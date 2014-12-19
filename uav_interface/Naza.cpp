#include "Naza.h"
#include "Adafruit_PWMServoDriver.h"
//225, 900
#define PWM_MIN 2250
//389, 1556
#define PWM_MAX 3890
//307, 1228
#define PWM_MIDDLE 3070

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
  
  _PWMDriver = new Adafruit_PWMServoDriver(servoDriverAddress);
  
  //delay(100);
  
  _PWMDriver->begin();
  
  //delay(100);
  
  _PWMDriver->setPWMFreq(500);
  
  _PWMDriver->setPWM(PITCH_CHANNEL, 0, PWM_MIDDLE);
  _PWMDriver->setPWM(ROLL_CHANNEL, 0, PWM_MIDDLE);
  _PWMDriver->setPWM(YAW_CHANNEL, 0, PWM_MIDDLE);
  _PWMDriver->setPWM(GAZ_CHANNEL, 0, PWM_MIN);
  
}

void Naza::ValuesChanged()
{
  uint16_t pulselengthPitch = map(_pitch, -1, 1, PWM_MIN, PWM_MAX);
  uint16_t pulselengthRoll = map(_roll, -1, 1, PWM_MIN, PWM_MAX);
  uint16_t pulselengthYaw = map(_yaw, -1, 1, PWM_MIN, PWM_MAX);
  uint16_t pulselengthGaz = map(_gaz, -1, 1, PWM_MIN, PWM_MAX);

  _PWMDriver->setPWM(PITCH_CHANNEL, 0, pulselengthPitch);
  _PWMDriver->setPWM(ROLL_CHANNEL, 0, pulselengthRoll);
  _PWMDriver->setPWM(YAW_CHANNEL, 0, pulselengthYaw);
  _PWMDriver->setPWM(GAZ_CHANNEL, 0, pulselengthGaz);
}

// TODO: Add timeout to interupts
int out = PWM_MIN;
void Naza::Update()
{
  while(out <= PWM_MAX)
  {
    _PWMDriver->setPWM(PITCH_CHANNEL, 0, out);
    _PWMDriver->setPWM(ROLL_CHANNEL, 0, out);
    _PWMDriver->setPWM(YAW_CHANNEL, 0, out);
    _PWMDriver->setPWM(GAZ_CHANNEL, 0, out);
    
    out ++;
    delay(10);
  }
  out = PWM_MAX;
  delay(1000);
  while(out >= PWM_MIN)
  {
    _PWMDriver->setPWM(PITCH_CHANNEL, 0, out);
    _PWMDriver->setPWM(ROLL_CHANNEL, 0, out);
    _PWMDriver->setPWM(YAW_CHANNEL, 0, out);
    _PWMDriver->setPWM(GAZ_CHANNEL, 0, out);
    
    out --;
    delay(10);
  }
  out = PWM_MIN;
  delay(1000);
}

