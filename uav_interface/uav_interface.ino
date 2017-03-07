#include <Wire.h>
#include <SPI.h>
//#include <SFE_MicroOLED.h>
#undef swap

#include "RequestHandler.h"
/*
#include "Sensor42.h"
#include "ActuatorExample.h"
#include "Naza.h"
#include "SRFSonar.h"
#include "AHRSplus.h"
#include "GR12.h"
#include "Vsense.h"
#include "FLIRLepton.h"
#include "LidarLite.h"
#include "StatusLED.h"
#include "Hedgehog.h"
#include "OLED.h"
*/

#include "Adafruit_TCS34725.h"
#include "AHRSplus.h"


#define HBLED 5
#define HBLED2 6
/*
#define SENS_SONAR false
#define SENS_MYAHRS false
#define SENS_GR12 false
#define SENS_HEDGEHOG false
#define SENS_VOLTAGE false
#define SENS_FLIR false
#define SENS_LIDAR false
#define SENS_TEST false

#define ACT_NAZA false
#define ACT_STATUSLED false
#define ACT_OLED false
*/
RequestHandler* requestHandler;

// create all sensor objects

Adafruit_TCS34725 * rgbSensor;
AHRSplus * ahrs;

/*
SRFSonar * sRFSonar;
GR12 * gR12;
AHRSplus * aHRS;
FLIRLepton * flir;
LidarLite * lidar;
VSense * vsensor;
Hedgehog * hedgehog;
Sensor42 * testsensor;
*/
//... feel free to add more ...
//... remember to add them to the list in setup()
//...


// create all actuator objects
/*
StatusLED * statusled;
Naza * naza;
OLED * oled;
*/
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
  //Wire.setClock(1000000);

  Serial.begin(115200);

  //SPI.begin(4);
  //SPI.setClockDivider(4, 6);

  //configure TwirreShield led
  pinMode(HBLED2,OUTPUT);
  pinMode(HBLED,OUTPUT);
  digitalWrite(HBLED, HIGH);  
  
  //delay to stabilize power and stuff
  delay(2500);
  digitalWrite(HBLED, LOW);


  //Initialize sensor objects
  rgbSensor = new Adafruit_TCS34725("RGBSensor");
  sensorList.Add(rgbSensor);

  ahrs = new AHRSplus("AHRS");
  sensorList.Add(ahrs);
  //Initialize actuator objects

  actuatorList.Add(rgbSensor);

  delay(100);
  digitalWrite(HBLED, HIGH);
  delay(500);
  digitalWrite(HBLED, LOW);
  
  //clear the serial buffer
  while (Serial.available())
  {
    Serial.read();
  }

  //create request handler
  requestHandler = new RequestHandler(&sensorList, &actuatorList, &Serial);

  delay(100);
}


int ctr = 0;
void loop()
{

  requestHandler->SendAndReceive();
  sensorList.UpdateAll();
  actuatorList.UpdateAll();
  
  //heartbeat on TwirreShield led
  if(ctr++ >= 1000){
    ctr = 0;
    digitalWrite(HBLED2, digitalRead(HBLED));
    digitalWrite(HBLED, !digitalRead(HBLED));
  }
  delay(1);

}
