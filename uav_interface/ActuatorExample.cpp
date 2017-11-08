 /* 
  * Copyright © 2017, Centre of Expertise Computer Vision & Data Science, NHL Stenden university of applied sciences
  * All rights reserved.
  * 
  * no warranty, no liability
  */

#include "ActuatorExample.h"

ActuatorExample::ActuatorExample(char* name, Stream* stream) : Device(name, "This actuator is used for testing purposes. A variable 'valueToSet' can be set.")
{
  //we add the variable so it can be changed
  _AddVariable("valueToSet", &_value);
  _stream = stream;

  _value = 0;
  _oldValue = 0;
}

void ActuatorExample::Update()
{
  if (_oldValue != _value)
  {
    //it will print the value as a string and add a new line 123 -> "123\n"
    _stream->println(_value);
    _oldValue = _value;
  }
}

