#include <Wire.h>

#include "RequestHandler.h"
#include "Sensor42.h"
#include "ActuatorExample.h"
#include "Naza.h"
#include "SRFSonar.h"
#include "AHRSplus.h"
#include "GR12.h"

RequestHandler* requestHandler;

// create all sensor objects
Sensor42 sensor42("sensor42");
//Sensor42 secondSensor42("secondSensor42");

SRFSonar * sRFSonar;

GR12 * gR12;

Naza * naza;

AHRSplus * aHRS;
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
  //use normal Wire as high-speed i2c (1MHz)
  //because Wire has strong 1.5k pull-ups built-in
  Wire.begin();
  Wire.setClock(1000000);
  
  //use Wire1 as normal i2c (100 KHz)
  Wire1.begin();
  Wire1.setClock(100000);
  
  SerialUSB.begin(115200);
  SerialUSB.setTimeout(50);
  //delay to stabilize power and stuff
  delay(2500);

  naza = Naza::Initialize("naza");
  //sRFSonar = new SRFSonar("sonar1", 120, SRF08);
  //aHRS = new AHRSplus("myAHRS+");
  gR12 = new GR12("gR12");

  //add all sensors created above
  //sensorList.Add(&sensor42);
  //sensorList.Add(&secondSensor42);
  //sensorList.Add(sRFSonar);
  //sensorList.Add(aHRS);
  sensorList.Add(gR12);
  //...
  
  //add all actuators created above
  //actuatorList.Add(&actuatorExample);
  actuatorList.Add(naza);
  //...


  //  clear the buffer
  while(SerialUSB.available())
  {
    SerialUSB.read();
  }
  
  //create request handler
  requestHandler = new RequestHandler(&sensorList, &actuatorList, &SerialUSB);
}

int ctr = 0;

void loop()
{
  requestHandler->SendAndReceive();
  sensorList.UpdateAll();
  actuatorList.UpdateAll();
}
