 /* 
  * Copyright © 2017, Centre of Expertise Computer Vision & Data Science, NHL Stenden university of applied sciences
  * All rights reserved.
  * 
  * no warranty, no liability
  */

#include "StatusLED.h"
#include "Arduino.h"

StatusLED::StatusLED(const char* name) : Device(name, "With this actuator you can control the RGB Status LED")
{
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);

  digitalWrite(LEDR, LOW);
  digitalWrite(LEDG, LOW);
  digitalWrite(LEDB, LOW);

  _AddVariable("red", &_red);
  _AddVariable("green", &_green);
  _AddVariable("blue", &_blue);
}

void StatusLED::ValuesChanged()
{
  digitalWrite(LEDR, _red);
  digitalWrite(LEDG, _green);
  digitalWrite(LEDB, _blue);  
}

void StatusLED::SetValue(int8_t red,int8_t green, int8_t blue)
{
  _red = red;
  _green = green;
  _blue = blue;

  ValuesChanged();
}

