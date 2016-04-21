#include <Wire.h>
#include <SPI.h>
//#include <SFE_MicroOLED.h>
#undef swap

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
#include "StatusLED.h"
#include "Hedgehog.h"

#define HBLED 23

#define SENS_SONAR true
#define SENS_MYAHRS true
#define SENS_GR12 true
#define SENS_HEDGEHOG false
#define SENS_VOLTAGE false
#define SENS_FLIR false
#define SENS_LIDAR false
#define SENS_TEST false

#define ACT_NAZA true
#define ACT_STATUSLED true
#define ACT_OLED false

RequestHandler* requestHandler;

// create all sensor objects
SRFSonar * sRFSonar;
GR12 * gR12;
AHRSplus * aHRS;
FLIRLepton * flir;
LidarLite * lidar;
VSense * vsensor;
Hedgehog * hedgehog;
Sensor42 * testsensor;

//... feel free to add more ...
//... remember to add them to the list in setup()
//...


// create all actuator objects
StatusLED * statusled;
Naza * naza;
//... feel free to add more ...
//... remember to add them to the list in setup()
//...

DeviceList sensorList;
DeviceList actuatorList;

//MicroOLED oled(45, 1);
void setup()
{
  //use normal Wire as high-speed i2c (1MHz)
  //because Wire has strong 1.5k pull-ups built-in
  Wire.begin();
  Wire.setClock(1000000);

  //use Wire1 as normal i2c (100 KHz)
  //oled.begin(); //stupid library
  Wire1.begin();
  Wire1.setClock(100000);

  SerialUSB.begin(115200);
  SerialUSB.setTimeout(50);
  Serial.begin(115200);

  //SPI.begin(4);
  //SPI.setClockDivider(4, 6);

  //configure TwirreShield led
  pinMode(HBLED,OUTPUT);
  digitalWrite(HBLED, HIGH);  
  
  //delay to stabilize power and stuff
  delay(2500);
  digitalWrite(HBLED, LOW);


  //Initialize sensor objects
  #if SENS_SONAR
    sRFSonar = new SRFSonar("sonar1", 120, SRF08);
    sensorList.Add(sRFSonar);
  #endif
  
  #if SENS_MYAHRS
    aHRS = new AHRSplus("myAHRS+");
    sensorList.Add(aHRS);
  #endif

  #if SENS_GR12
    gR12 = new GR12("gR12");
    sensorList.Add(gR12);
  #endif

  #if SENS_HEDGEHOG
    hedgehog = new Hedgehog("Hedgehog");
    sensorList.Add(hedgehog);
  #endif

  #if SENS_VOLTAGE
    vsensor = new VSense("vbat");  //vmax calculated from TwirreShield voltage divider
    sensorList.Add(vsensor);
  #endif

  #if SENS_FLIR 
    flir = new FLIRLepton("flir",4,5);
    sensorList.Add(flir);
  #endif

  #if SENS_LIDAR
    lidar = new LidarLite("Lidar",0x62);
    sensorList.Add(lidar);
  #endif

  #if SENS_TEST
    testsensor = new Sensor42("sensor42");
    sensorList.Add(testsensor);
  #endif


  //Initialize actuator objects
  #if ACT_NAZA 
    #if ACT_GR12
      naza = Naza::Initialize("naza", gR12);
    #else
      naza = Naza::Initialize("naza");
    #endif
    actuatorList.Add(naza);
  #endif

  #if ACT_STATUSLED 
    statusled = new StatusLED("RGB_LED");
    actuatorList.Add(statusled);    
  #endif

  #if ACT_OLED
    
  #endif
  



  delay(100);
  digitalWrite(HBLED, HIGH);
  delay(500);
  digitalWrite(HBLED, LOW);
  
  //clear the serial buffer
  while (SerialUSB.available())
  {
    SerialUSB.read();
  }

  //create request handler
  requestHandler = new RequestHandler(&sensorList, &actuatorList, &SerialUSB);

  delay(100);
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
  if(ctr == 1000)
  {
    ctr = 0;
    digitalWrite(HBLED, on);
    on = 1 - on; 
  }
}
