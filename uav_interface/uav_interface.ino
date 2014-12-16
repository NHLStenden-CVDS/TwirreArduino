#include <Wire.h>

#include "RequestHandler.h"
#include "Sensor42.h"
#include "ActuatorExample.h"

RequestHandler* requestHandler;

// create all sensor objects
Sensor42 sensor42("sensor42");
Sensor42 secondSensor42("secondSensor42");
//... feel free to add more ...
//... remember to add them to the list in setup()
//...

// create all actuator objects
ActuatorExample actuatorExample("actuatorExample", &Serial);
//... feel free to add more ...
//... remember to add them to the list in setup()
//...

DeviceList sensorList;
DeviceList actuatorList;



void setup()
{
  Serial.begin(115200);

  //add all sensors created above
  sensorList.Add(&sensor42);
  sensorList.Add(&secondSensor42);
  //...
  
  //add all actuators created above
  actuatorList.Add(&actuatorExample);
  //...

  //create request handler
  requestHandler = new RequestHandler(&sensorList, &actuatorList, &Serial);
}

void loop()
{
  requestHandler->SendAndReceive();
  sensorList.UpdateAll();
  actuatorList.UpdateAll();
}
