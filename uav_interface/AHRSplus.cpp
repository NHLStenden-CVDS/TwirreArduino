#include "AHRSplus.h"
#include <Wire.h>
#define ADDRESS 0x20

AHRSplus::AHRSplus(char* name) : Device(name, "This is the myAHRS+ sensor. It is a fancy IMU or 'attitude heading reference system'")
{
  _AddVariable("accX", &_accX);
  _AddVariable("accY", &_accY);
  _AddVariable("accZ", &_accZ);
  _AddVariable("gyroX", &_gyroX);
  _AddVariable("gyroY", &_gyroY);
  _AddVariable("gyroZ", &_gyroZ);
  _AddVariable("magX", &_magX);
  _AddVariable("magY", &_magY);
  _AddVariable("magZ", &_magZ);
  _AddVariable("temp", &_temp);
  _AddVariable("roll", &_roll);
  _AddVariable("pitch", &_pitch);
  _AddVariable("yaw", &_yaw);
  _AddVariable("quaternionX", &_quaternionX);
  _AddVariable("quaternionY", &_quaternionY);
  _AddVariable("quaternionZ", &_quaternionZ);
  _AddVariable("quaternionW", &_quaternionW);
  
}

void AHRSplus::Update()
{
	uint8_t address = 0x10;
  _accX = registerRead(2 * (address++));
  _accY = registerRead(2 * (address++));
  _accZ = registerRead(2 * (address++));
  _gyroX = registerRead(2 * (address++));
  _gyroY = registerRead(2 * (address++));
  _gyroZ = registerRead(2 * (address++));
  _magX = registerRead(2 * (address++));
  _magY = registerRead(2 * (address++));
  _magZ = registerRead(2 * (address++));
  _temp = registerRead(2 * (address++));
  _roll = registerRead(2 * (address++));
  _pitch = registerRead(2 * (address++));
  _yaw = registerRead(2 * (address++));
  _quaternionX = registerRead(2 * (address++));
  _quaternionY = registerRead(2 * (address++)); 
  _quaternionZ = registerRead(2 * (address++));
  _quaternionW = registerRead(2 * (address++));
  
}

uint16_t AHRSplus::registerRead(uint8_t  registerAddress)
{  
  TWI_StartRead(WIRE_INTERFACE, ADDRESS, registerAddress,1);
  TWI_SendSTOPCondition(WIRE_INTERFACE);
  while(!TWI_ByteReceived(WIRE_INTERFACE));
  uint8_t lsb = TWI_ReadByte(WIRE_INTERFACE);
  while(!TWI_TransferComplete(WIRE_INTERFACE));
  
  TWI_StartRead(WIRE_INTERFACE, ADDRESS, registerAddress + 1,1);
  TWI_SendSTOPCondition(WIRE_INTERFACE);
  while(!TWI_ByteReceived(WIRE_INTERFACE));
  uint8_t msb = TWI_ReadByte(WIRE_INTERFACE);
  while(!TWI_TransferComplete(WIRE_INTERFACE));
  
  uint16_t combined = 0;
  combined = (msb << 8) | lsb;
  return combined;
}
