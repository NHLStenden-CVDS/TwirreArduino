#include "AHRSplus.h"
#include <Wire.h>
#define ADDRESS 0x20
#define I2C_READ_TIMEOUT 10000
#define LOOP_SKIP_MICROSECONDS 10000

AHRSplus::AHRSplus(char* name) : Device(name, "This is the myAHRS+ sensor. It is a fancy IMU or 'attitude heading reference system'")
{
  _time = micros();
  
  _AddVariable("accX", &_normalizedData.accX);
  _AddVariable("accY", &_normalizedData.accY);
  _AddVariable("accZ", &_normalizedData.accZ);
  _AddVariable("gyroX", &_normalizedData.gyroX);
  _AddVariable("gyroY", &_normalizedData.gyroY);
  _AddVariable("gyroZ", &_normalizedData.gyroZ);
  _AddVariable("magX", &_normalizedData.magX);
  _AddVariable("magY", &_normalizedData.magY);
  _AddVariable("magZ", &_normalizedData.magZ);
  _AddVariable("temp", &_normalizedData.temp);
  _AddVariable("roll", &_normalizedData.roll);
  _AddVariable("pitch", &_normalizedData.pitch);
  _AddVariable("yaw", &_normalizedData.yaw);
  _AddVariable("quaternionX", &_normalizedData.quaternionX);
  _AddVariable("quaternionY", &_normalizedData.quaternionY);
  _AddVariable("quaternionZ", &_normalizedData.quaternionZ);
  _AddVariable("quaternionW", &_normalizedData.quaternionW);
}


void AHRSplus::Update()
{
  if(micros() - _time >= LOOP_SKIP_MICROSECONDS)
  {
    readBytes(reinterpret_cast<uint8_t*>(&_data), 0x22, sizeof(AHRSdata));
    _time = micros();
    dataToNormalizedData();
  }
}


void AHRSplus::dataToNormalizedData()
{
  _normalizedData.accX = (16.0f / 32767.0f) * static_cast<float>(_data.accX);
  _normalizedData.accY = (16.0f / 32767.0f) * static_cast<float>(_data.accY);
  _normalizedData.accZ = (16.0f / 32767.0f) * static_cast<float>(_data.accZ);
  _normalizedData.gyroX = (2000.0f / 32767.0f) * static_cast<float>(_data.gyroX);
  _normalizedData.gyroY = (2000.0f / 32767.0f) * static_cast<float>(_data.gyroY);
  _normalizedData.gyroZ = (2000.0f / 32767.0f) * static_cast<float>(_data.gyroZ);
  _normalizedData.magX =  static_cast<float>(_data.magX);
  _normalizedData.magY =  static_cast<float>(_data.magY);
  _normalizedData.magZ =  static_cast<float>(_data.magZ);
  _normalizedData.temp = (200.0f / 32767.0f) * static_cast<float>(_data.temp);
  _normalizedData.roll = (180.0f / 32767.0f) * static_cast<float>(_data.roll);
  if(_normalizedData.roll >= 180.0f) _normalizedData.roll -= 360.0f;
  _normalizedData.pitch = (180.0f / 32767.0f) * static_cast<float>(_data.pitch);
  if(_normalizedData.pitch >= 180.0f) _normalizedData.pitch -= 360.0f;
  _normalizedData.yaw = (180.0f / 32767.0f) * static_cast<float>(_data.yaw);
  if(_normalizedData.yaw >= 180.0f) _normalizedData.yaw -= 360.0f;
  _normalizedData.quaternionX = static_cast<float>(_data.quaternionX) / 32767.0f;
  _normalizedData.quaternionY = static_cast<float>(_data.quaternionY) / 32767.0f;
  _normalizedData.quaternionZ = static_cast<float>(_data.quaternionZ) / 32767.0f;
  _normalizedData.quaternionW = static_cast<float>(_data.quaternionW) / 32767.0f;
}


bool waitByteReceived(unsigned long timeout)
{
  unsigned long started = micros();
  while(!TWI_ByteReceived(WIRE1_INTERFACE) && (micros() - started < timeout));
  
  return (micros() - started < timeout);
}


bool waitTransferComplete(unsigned long timeout)
{
  unsigned long started = micros();
  while(!TWI_TransferComplete(WIRE1_INTERFACE) && (micros() - started < timeout));
  
  return (micros() - started < timeout);
}


uint16_t AHRSplus::readBytes(uint8_t* dest, uint8_t regAddress, int count)
{
   TWI_StartRead(WIRE1_INTERFACE, ADDRESS, regAddress,1);
   
   //read all bytes except last one
   for(int i = 0; i < (count - 1); i++)
   {
     if(!waitByteReceived(I2C_READ_TIMEOUT)) return i;
     dest[i] = TWI_ReadByte(WIRE1_INTERFACE);
   }
   
   //set stop condition before reading final byte
   TWI_SendSTOPCondition(WIRE1_INTERFACE);
   if(!waitByteReceived(I2C_READ_TIMEOUT)) return count - 1;
   dest[count - 1] = TWI_ReadByte(WIRE1_INTERFACE);
   
   //wait for completion of transfer
   waitTransferComplete(I2C_READ_TIMEOUT);
   
   return count;
}
