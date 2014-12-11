#include "ActuatorExample.h"

ActuatorExample::ActuatorExample(char* name, Stream* stream) : Device(name, "This actuator is used for testing purposes. A variable 'valueToSet' can be set.")
{
  _AddVariable("valueToSet", &_value);
  _stream = stream;
  
  _value = 0;
  _oldValue = 0;
}

void ActuatorExample::Update()
{
  if(_oldValue != _value)
  {
    _stream->println(_value);
    _oldValue = _value;
  }
}

