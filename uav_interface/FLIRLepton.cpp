 /* 
  * Copyright © 2017, Centre of Expertise Computer Vision & Data Science, NHL Stenden university of applied sciences
  * All rights reserved.
  * 
  * no warranty, no liability
  */

#include "FLIRLepton.h"
#include "Arduino.h"

#include <SPI.h>
#include <cstring>

FLIRLepton::FLIRLepton(char* name, int slavePin, int pwrPin) : Device(name, "This is a FLIR Lepton Thermal Camera")
{
  _slavePin = slavePin;
  _pwrPin = pwrPin;


  _imgSize = (uint32_t) ( _width * _height);

  _AddVariable("image", _img, &_imgSize);
  _AddVariable("height", &_height);
  _AddVariable("width", &_width);


  pinMode(pwrPin, OUTPUT);
  turnOn();
  delay(1000);
  turnOff();
  delay(1000);
  turnOn();
}
/*
void FLIRLepton::Update()
{
  int counter = 0;
  for(int i = 0; i < 15; i++)
  {
    counter = lepton_Transfer();
    SerialUSB.println(counter);

    if(counter == -2)delay(100);
    else if(counter == _height - 1){
      print_img();
      delay(100);
      break;
    }
  }  
}
*/



void FLIRLepton::Update()
{ 
  int counter = 0;
  int lastCounter = 0;
  unsigned long curTime = millis(); //Current time;
  
  switch(_state){
    case READING:
    
      for(int i = 0; i < 15; i++)
      {
        counter = lepton_Transfer();
        //delay(1);
        //delayMicroseconds(50);
        //if(counter == -1 || counter == 0 || counter == 32 || counter == 33)_errCount++;
        //else if(counter > 0)_errCount = 0;
        if(_errCount > 300000)_state = CYCLE;
        if(counter == -2)_errCount+=1000;
        
        _errCount++;
        if(counter == _height - 1){
          _errCount = 0;
          //print_img();
          _state = WAIT;
          break;
        }
      } 
      //SerialUSB.println(counter);

      break;
    case WAIT:     //Just read an image, waiting for next read period
      _lastOp = curTime;
      _state = IDLE;  
      break;
    case IDLE:
      if( curTime > _lastOp + 100)_state = READING;
      break;
    case CYCLE:
      _errCount = 0;
      _lastOp = curTime;
      _state = CYCLE2;
      turnOff();
      break;
    case CYCLE2:
      if(curTime > _lastOp + 3000){
        turnOn();
        _lastOp = curTime;
        _state = CYCLE3;        
      }
      break;
    case CYCLE3:
      if(curTime > _lastOp + 50){
        _lastOp = curTime;
        _state = READING;        
      }
      break;

  }

  

}



int FLIRLepton::lepton_Transfer(){
  int frame_nr = -1;
  for(int i = 0 ; i < PACKET_SIZE ; i++){
    _frame_packet[i] = SPI.transfer(4, 0x00, SPI_CONTINUE);
  }

  if(((_frame_packet[0]&0xf) != 0x0f)){
    frame_nr = _frame_packet[1];
      
    if(frame_nr < _height){
      for(int i = 0 ; i < _width ; i++){
        _img[frame_nr * _width + i] = (_frame_packet[2*i+4] << 8 | _frame_packet[2*i+5]);       
      }
    }
    else return -2;
  }
  //SerialUSB.println(frame_nr);
  return frame_nr;
}



void FLIRLepton::print_img(){
  String result = "";
  
  for(int i = 0 ; i < 60 ; i++){
    for(int j = 0 ; j < 80 ; j++){
      result += _img[i * _height + j];
      result += " ";
    }
    result += "\r\n";
    //SerialUSB.println("");
  }
  SerialUSB.println(result);
  SerialUSB.println("DONE");
}


void FLIRLepton::turnOn()
{
  if(_powerState)return;
  digitalWrite(_pwrPin, HIGH);
  _powerState = true;
}

void FLIRLepton::turnOff()
{
  if(!_powerState)return;
  digitalWrite(_pwrPin, LOW);
  _powerState = false;
}


