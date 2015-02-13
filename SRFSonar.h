#ifndef SRFSONAR_H_
#define SRFSONAR_H_

// settings for SRF08
#define SRF08_START_ADDR 0x01
#define SRF08_READ_BYTES 3
#define SRF08_READ_VALUES 1

// settings for SRF02
#define SRF02_START_ADDR 0x02
#define SRF02_READ_BYTES 4
#define SRF02_READ_VALUES 2

#include "Device.h"

enum SRFType
{
  SRF08,
  SRF02
};

class SRFSonar : public Device
{
  public:
    /**
    * Gain: Reduce if the sonar picks up 'false' echoes. Increase if the sonar fails to receive
    * the correct echo. (0x1F is max and default, but caused severe ghost echo problems).
    * Range:lower values will cause lower maximum range, but increases refresh rate. Lower ranges 
    * require a lower gain value. Default = 0xFF (11 meters). As the SRF08 cannot reliably range beyond
    * 6 meters, this setting is recommended to be 0x8C (6 meters).
    */
    SRFSonar(char *name, uint8_t I2CAddress, SRFType type, uint8_t gain = 0x00, uint8_t range = 0x8C);
    void Update();
    void changeAddress(uint8_t address);

  private:
    uint16_t *_lastReadingRaw;
    uint16_t _lastReadingRawLength;
    uint8_t _I2CAddress;
    uint8_t _startAddress;
    uint8_t _nrOfBytesToRead;
    bool _hasLightSensor;
    uint8_t _lightSensorValue;
        
    void startUltrasonicRanging();
    void readUltrasonicSensorValue();
};

#endif
