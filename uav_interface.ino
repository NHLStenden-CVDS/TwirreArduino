#include <Wire.h>

#include "RequestHandler.h"
#include "Sensor42.h"
#include "ActuatorExample.h"
#include "Naza.h"
#include "SRFSonar.h"

RequestHandler* requestHandler;

// create all sensor objects
Sensor42 sensor42("sensor42");
Sensor42 secondSensor42("secondSensor42");

SRFSonar * sRFSonar;

Naza * naza;
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
  Wire.begin();
  Serial.begin(115200);

  naza = new Naza("de naza flightcontroller", 0x41);
  sRFSonar = new SRFSonar("sonar1", 120, SRF08);

  //add all sensors created above
  //sensorList.Add(&sensor42);
  //sensorList.Add(&secondSensor42);
  sensorList.Add(sRFSonar);
  //...
  
  //add all actuators created above
  //actuatorList.Add(&actuatorExample);
  actuatorList.Add(naza);
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
