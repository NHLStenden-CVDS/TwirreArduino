#include <Wire.h>
#include <SPI.h>

#include "RequestHandler.h"
#include "Sensor42.h"
#include "ActuatorExample.h"
#include "Naza.h"
#include "SRFSonar.h"
#include "AHRSplus.h"
#include "GR12.h"
#include "Vsense.h"
#include "FLIRLepton.h"
#include "LidarLite.h"

RequestHandler* requestHandler;

// create all sensor objects
SRFSonar * sRFSonar;
GR12 * gR12;
Naza * naza;
AHRSplus * aHRS;
FLIRLepton * flir;
LidarLite * lidar;

//VSense * vsensor;

//... feel free to add more ...
//... remember to add them to the list in setup()
//...

// create all actuator objects
//... feel free to add more ...
//... remember to add them to the list in setup()
//...

DeviceList sensorList;
DeviceList actuatorList;


//unsigned long lastLed = 0;

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
  Serial.begin(115200);

  SPI.begin(4);
  SPI.setClockDivider(4, 6);
  
  //delay to stabilize power and stuff
  delay(2500);

  naza = Naza::Initialize("naza");
  sRFSonar = new SRFSonar("sonar1", 120, SRF08);
  aHRS = new AHRSplus("myAHRS+");
  gR12 = new GR12("gR12");
  vsensor = new VSense(A0, 0, 24.37578, "vbat");  //vmax calculated from TwirreShield voltage divider
  flir = new FLIRLepton("flir",4,5);
  lidar = new LidarLite("Lidar",0x62);
  
  //add all sensors created above
  sensorList.Add(sRFSonar);
  sensorList.Add(aHRS);
  sensorList.Add(gR12);
  sensorList.Add(vsensor);
  sensorList.Add(flir);
  sensorList.Add(lidar);
  
  //add all actuators created above
 // actuatorList.Add(naza);
  
  //configure TwirreShield led
  pinMode(48,OUTPUT);
  
  //clear the serial buffer
  while (SerialUSB.available())
  {
    SerialUSB.read();
  }

  //create request handler
  requestHandler = new RequestHandler(&sensorList, &actuatorList, &SerialUSB);
}

int on = 1;
int ctr = 0;
void loop()
{
  requestHandler->SendAndReceive();
  sensorList.UpdateAll();
  actuatorList.UpdateAll();
  
  //heartbeat on TwirreShield led
  ctr++;
  if(ctr == 50)
  {
    ctr = 0;
    digitalWrite(48, on);
    on = 1 - on; 
  }
}
