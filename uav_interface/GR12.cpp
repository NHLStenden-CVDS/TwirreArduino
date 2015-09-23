#include "GR12.h"

#define YAW_PORT 12
#define PITCH_PORT 3
#define ROLL_PORT 2
#define GAZ_PORT 11
#define IS_AUTONOMOUS_PORT 10

HANDLE_CHANGE_PORT(Yaw)
HANDLE_CHANGE_PORT(Pitch)
HANDLE_CHANGE_PORT(Roll)
HANDLE_CHANGE_PORT(Gaz)
HANDLE_CHANGE_PORT(IsAutonomous)

GR12::GR12(char* name) : Device(name, "lol")//"This sensor can read the stick values and is automous switch from the GR12 receiver. Pitch, roll, gaz and yaw values are returned between -1 and 1. isAutonomous is returned in a byte, 1 for true, 0 for false")
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

void GR12::Update()
{
  
}

void GR12::OnRequest()
{
  _yaw = dutyCycleToStickValue(lastPulseDurationYaw, 1098.6328125, 1899.4140625f);
  _pitch = dutyCycleToStickValue(lastPulseDurationPitch, 1098.6328125, 1899.4140625f);
  _roll = dutyCycleToStickValue(lastPulseDurationRoll, 1098.6328125, 1899.4140625f);
  _gaz = dutyCycleToStickValue(lastPulseDurationGaz, 1098.6328125, 1899.4140625f);
  
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
