/*
 * Twirre: architecture for autonomous UAVs using interchangeable commodity components
 *
 * Copyright© 2017 Centre of expertise in Computer Vision & Data Science, NHL Stenden University of applied sciences
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef SRFSONAR_H_
#define SRFSONAR_H_

// settings for SRF08
#define SRF08_START_ADDR 0x01
#define SRF08_READ_BYTES 3
#define SRF08_READ_VALUES 1
//retransmit gain/range every X readings
#define SRF08_SETTINGS_RETRANSMIT 400

// settings for SRF02
#define SRF02_START_ADDR 0x02
#define SRF02_READ_BYTES 4
#define SRF02_READ_VALUES 2

// filtering settings
#define FILTER_WINDOW_SIZE 1

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
    SRFSonar(const char *name, uint8_t I2CAddress, SRFType type, uint8_t gain = 0x00, uint8_t range = 0x8C);
    void Update();
    void changeAddress(uint8_t address);

  private:
    uint32_t _timestamp;  //timestamp for last reading
    uint16_t *_lastReadingRaw;
    uint16_t _firstDistance;
    float _firstDistanceCorrected;  //unit corrected to metres
    uint32_t _lastReadingRawLength;
    uint8_t _I2CAddress;
    uint8_t _startAddress;
    uint8_t _nrOfBytesToRead;
    bool _hasLightSensor;
    uint8_t _lightSensorValue;
    

    // window size of the median filter

    uint8_t _lastReadingPointer = 0;
    uint16_t *_lastReadingsBuffer;
    
    uint8_t _gain;
    uint8_t _range;
    uint16_t _retransmitCtr;
    
    SRFType _type;
        
    void startUltrasonicRanging();
    void readUltrasonicSensorValue();
};

#endif
