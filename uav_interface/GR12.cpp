 /* 
  * Copyright © 2017, Centre of Expertise Computer Vision & Data Science, NHL Stenden university of applied sciences
  * All rights reserved.
  * 
  * no warranty, no liability
  */

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
//was 1098.6328125
#define GR12_PULSE_MIN 1100.0
//was 1899.4140625
#define GR12_PULSE_MAX 1900.0
void GR12::Update()
{
  _yaw = dutyCycleToStickValue(lastPulseDurationYaw, GR12_PULSE_MIN, GR12_PULSE_MAX);
  CLAMP(_yaw)
  _pitch = dutyCycleToStickValue(lastPulseDurationPitch, GR12_PULSE_MIN, GR12_PULSE_MAX);
  CLAMP(_pitch)
  _roll = dutyCycleToStickValue(lastPulseDurationRoll, GR12_PULSE_MIN, GR12_PULSE_MAX);
  CLAMP(_roll)
  _gaz = dutyCycleToStickValue(lastPulseDurationGaz, GR12_PULSE_MIN, GR12_PULSE_MAX);
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
  return (pulseTime - minPulseTime) * (1.0 - -1.0) / (maxPulseTime - minPulseTime) + -1.0; 
}
