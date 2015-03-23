#include <Wire.h>

#include "RequestHandler.h"
#include "Sensor42.h"
#include "ActuatorExample.h"
#include "Naza.h"
#include "SRFSonar.h"
#include "AHRSplus.h"

RequestHandler* requestHandler;

// create all sensor objects
Sensor42 sensor42("sensor42");
Sensor42 secondSensor42("secondSensor42");

SRFSonar * sRFSonar;

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
  pinMode(22, OUTPUT);
  digitalWrite(22, HIGH);
  
  Wire.begin();
  Wire1.begin();
  Wire1.setClock(200000);
  
  SerialUSB.begin(115200);
  //delay to stabilize power and stuff
  delay(1500);

  //naza = Naza::Initialize("The naza flight controller");
  sRFSonar = new SRFSonar("sonar1", 120, SRF08);
  aHRS = new AHRSplus("myAHRS+");

  //add all sensors created above
  sensorList.Add(&sensor42);
  //sensorList.Add(&secondSensor42);
  sensorList.Add(sRFSonar);
  sensorList.Add(aHRS);
  //...
  
  //add all actuators created above
  //actuatorList.Add(&actuatorExample);
 // actuatorList.Add(naza);
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
  
  if(ctr == 0)
    digitalWrite(22, LOW);
   
   ctr++;
   
  if(ctr >= 44)
  {
    ctr = -6;
    digitalWrite(22, HIGH);
  }
  
  
  requestHandler->SendAndReceive();
  sensorList.UpdateAll();
  actuatorList.UpdateAll();
}
