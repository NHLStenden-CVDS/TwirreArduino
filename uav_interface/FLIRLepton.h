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

#ifndef FLIRLEPTON_H_
#define FLIRLEPTON_H_

#define PACKET_SIZE 164
#define SPI_CLOCKDIV 4

#define IMG_WIDTH 80
#define IMG_HEIGHT 60

#include "Device.h"


enum FLIRState {
  READING, 
  WAIT,
  IDLE,     //Just read an image, waiting for next read period
  CYCLE,
  CYCLE2,
  CYCLE3
};



class FLIRLepton : public Device
{
  public:
    FLIRLepton(char *name, int slavePin, int pwrPin);
    void Update();
    
    void print_img();
    void turnOn();
    void turnOff();

  private:
    int lepton_Transfer();

    unsigned long _lastOp = 0;
    uint32_t _errCount = 0;
    FLIRState _state = READING;
    
    uint32_t _imgSize;    
    uint8_t _width = IMG_WIDTH;
    uint8_t _height = IMG_HEIGHT;
    uint16_t _img[IMG_WIDTH*IMG_HEIGHT];
    uint8_t _frame_packet[PACKET_SIZE];
    int _slavePin, _pwrPin;
    bool _powerState;

};

#endif
