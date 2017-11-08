 /* 
  * Copyright © 2017, Centre of Expertise Computer Vision & Data Science, NHL Stenden university of applied sciences
  * All rights reserved.
  * 
  * no warranty, no liability
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
