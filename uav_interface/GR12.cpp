#include "GR12.h"

#define YAW_PORT 2
#define PITCH_PORT 4
#define ROLL_PORT 5
#define GAZ_PORT 3
#define IS_AUTONOMOUS_PORT 10

HANDLE_CHANGE_PORT(Yaw)
HANDLE_CHANGE_PORT(Pitch)
HANDLE_CHANGE_PORT(Roll)
HANDLE_CHANGE_PORT(Gaz)
HANDLE_CHANGE_PORT(IsAutonomous)

GR12::GR12(const char* name) : Device(name, "lol")//"This sensor can read the stick values and is automous switch from the GR12 receiver. Pitch, roll, gaz and yaw values are returned between -1 and 1. isAutonomous is returned in a byte, 1 for true, 0 for false")
{
  _AddVariable("isAutonomous", &_isAutonomous);
  _AddVariable("pitch", &_pitch);
  _AddVariable("roll", &_roll);
  _AddVariable("gaz", &_gaz);
  _AddVariable("yaw", &_yaw);
  
  attachInterrupt(YAW_PORT, handleChangePortYaw, CHANGE);
  attachInterrupt(PITCH_PORT, handleChangePortPitch, CHANGE);
  attachInterrupt(ROLL_PORT, handleChangePortRoll, CHANGE);
  attachInterrupt(GAZ_PORT, handleChangePortGaz, CHANGE);
  attachInterrupt(IS_AUTONOMOUS_PORT, handleChangePortIsAutonomous, CHANGE);
}

double GR12::getPitch()
{
  return _pitch;
}

double GR12::getYaw()
{
  return _yaw;
}

double GR12::getGaz()
{
  return _gaz;
}

double GR12::getRoll()
{
  return _roll;
}

#define CLAMP(X) (X) = ((X) < -1.0f) ? -1.0f : ((X) > 1.0f) ? 1.0f : (X);
void GR12::Update()
{
  _yaw = dutyCycleToStickValue(lastPulseDurationYaw, 1098.6328125, 1899.4140625f);
  
  CLAMP(_yaw)
  _pitch = dutyCycleToStickValue(lastPulseDurationPitch, 1098.6328125, 1899.4140625f);
  _pitch += 0.093;
  CLAMP(_pitch)
  _roll = dutyCycleToStickValue(lastPulseDurationRoll, 1098.6328125, 1899.4140625f);
  _roll += 0.093;
  CLAMP(_roll)
  _gaz = dutyCycleToStickValue(lastPulseDurationGaz, 1098.6328125, 1899.4140625f);
  _gaz += 0.093;
  CLAMP(_gaz)
  
  if(lastPulseDurationIsAutonomous > 1500)
  {
    _isAutonomous = 1;
  }
  else
  {
    _isAutonomous = 0;
  }
}

double GR12::dutyCycleToStickValue(uint64_t pulseTime, double minPulseTime, double maxPulseTime)
{
  return (pulseTime - minPulseTime) * (1.0f - -1.0f) / (maxPulseTime - minPulseTime) + -1.0f; 
}
