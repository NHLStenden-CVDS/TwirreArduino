#include "SRFSonar.h"
#include <cstring>
#include <Wire.h>

SRFSonar::SRFSonar(char* name, uint8_t I2CAddress, SRFType type, uint8_t gain, uint8_t range) : Device(name, "This is a ultrasonic sensor. It can be used to measure distances.")
{
  _I2CAddress = I2CAddress;
  
  // reserve space for the raw values
  switch(type)
  { 
    case SRF08:
      _lastReadingRawLength = SRF08_READ_VALUES;
      _startAddress = SRF08_START_ADDR;
      _nrOfBytesToRead = SRF08_READ_BYTES;
      _hasLightSensor = true;
      break;
    case SRF02:
      _lastReadingRawLength = SRF02_READ_VALUES;
      _startAddress = SRF02_START_ADDR;
      _nrOfBytesToRead = SRF02_READ_BYTES;
      _hasLightSensor = false;
      break;
  }
  _lastReadingRaw = (uint16_t *)malloc(sizeof(uint16_t) * _lastReadingRawLength);
  
  _AddVariable("firstDistance", _lastReadingRaw);
  _AddVariable("distanceValues", _lastReadingRaw, &_lastReadingRawLength);
  if(_hasLightSensor)
  {
    _AddVariable("lightSensor", &_lightSensorValue); 
  }
  
  //SRF08: setup gain and range
  if(type == SRF08)
  {
    Wire1.beginTransmission(_I2CAddress); // transmit to SRF08
    Wire1.write(byte(0x01));      // sets register pointer to gain register
    
    //write gain and range settings to sonar
    Wire1.write(byte(gain));
    Wire1.write(byte(range));
    
    Wire1.endTransmission();      // stop transmitting
  }
  
  startUltrasonicRanging();
}

void SRFSonar::Update()
{
  //Read register 0 (version reg) of sonar. If the sonar is currently ranging this read is specified to return 0xFF. 
  Wire1.beginTransmission(_I2CAddress);
  Wire1.write(byte(0x00));                 //set register pointer to reg 0
  Wire1.endTransmission();
  
  Wire1.requestFrom((int)_I2CAddress, 1);
  unsigned char complete = Wire1.read();   //read register
 // Serial.println(complete);
  
  //check if sonar is idle
  if(complete != 0xFF)
  {
   // delay(3);
    //read most recent data from sonar
    readUltrasonicSensorValue();
    //start new ranging
    startUltrasonicRanging();
  }
}

// Function which gives the ultrasonic sensor the instruction to start a measurement 
void SRFSonar::startUltrasonicRanging()
{
  // step 1: instruct sensor to read echoes
  Wire1.beginTransmission(_I2CAddress); //transmit to sonar address
  Wire1.write(byte(0x00));      // sets register pointer to the command register (0x00)  
  Wire1.write(byte(0x51));      // command sensor to measure in "centimeters" (0x51) 
                               // use 0x50 for inches
                               // use 0x52 for ping microseconds
  Wire1.endTransmission();      // stop transmitting
} 

void SRFSonar::changeAddress(uint8_t address)
{
  Wire1.beginTransmission(_I2CAddress);
  Wire1.write(byte(0x00));
  Wire1.write(byte(0xA0));
  Wire1.endTransmission(); 
  
  Wire1.beginTransmission(_I2CAddress);
  Wire1.write(byte(0x00));
  Wire1.write(byte(0xAA));
  Wire1.endTransmission();
  
  Wire1.beginTransmission(_I2CAddress);
  Wire1.write(byte(0x00));
  Wire1.write(byte(0xA5));
  Wire1.endTransmission();
  
  Wire1.beginTransmission(_I2CAddress);
  Wire1.write(byte(0x00));
  Wire1.write(address);
  Wire1.endTransmission();
  
  _I2CAddress = address;
}

// Function which reads the value from the ultrasonic sensor.
 void SRFSonar::readUltrasonicSensorValue()
 {
   // step 3: instruct sensor to return a particular echo reading
  Wire1.beginTransmission(_I2CAddress); // transmit to sonar
  Wire1.write(byte(_startAddress));      // sets register pointer to required register
  Wire1.endTransmission();      // stop transmitting

  // step 4: request reading from sensor
  Wire1.requestFrom(_I2CAddress, (uint8_t)_nrOfBytesToRead);    // request 2 bytes from slave device #112
  
 // Wire.read();
  // step 5: receive reading from sensor
  if(_nrOfBytesToRead <= Wire1.available())    // if two bytes were received
  {
    if(_hasLightSensor)
    {
      _lightSensorValue = Wire1.read();
    }
    
    for(int i = 0; i < _lastReadingRawLength; i++)
    {
      _lastReadingRaw[i] = Wire1.read();
      _lastReadingRaw[i] = _lastReadingRaw[i] << 8;
      _lastReadingRaw[i] |= Wire1.read();
    }
  }
}
