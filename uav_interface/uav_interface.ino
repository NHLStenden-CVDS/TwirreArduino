/*
 * Twirre: architecture for autonomous UAVs using interchangeable commodity components
 *
 * Copyright© 2017 Centre of expertise in Computer Vision & Data Science, NHL Stenden University of applied sciences
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


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
#include "OLED.h"


/*
 * Main file for Arduino code. Setup() is executed on board powerup or reset (will setup required sensors, and Loop() is repeated infinitely after that.
 */


//pin to which the heartbeat led is connected
#define HBLED 23
//number of loop iterations before heartbeat led toggles state. Adjust this for processor load per loop
#define HBLED_TOGGLE_COUNT 1000

/* 
 * sensor configuration 
 */
//SRF08 ultrasonic sensor (i2c bus)
#define SENS_SONAR true
#define SENS_SONAR_ADDR 120
//MyAHRS+ attitude heading reference system (magnetometer/compass, accelerometer, gyroscope) (i2c bus)
#define SENS_MYAHRS false
//Marvelmind ultrasonic positioning system (i2c bus)
#define SENS_HEDGEHOG false
//FLIR Lepton thermographic camera (not recommended, the Lepton is very timing-sensitive and should be attached to fully dedicated microcontroller) (i2c bus)
#define SENS_FLIR false
//Lidar-Lite 1D laser ranging system (i2c bus, but has a special connector on TwirreShield because a ~600µF capacitor is recommended for the 
#define SENS_LIDAR false
//Test sensor which should always report a value of 42
#define SENS_TEST false
//following sensors are present on the arduino, so typically can be enabled always
//voltage sensor for a 4-cell LiPo balance connector (voltage dividers are needed!, which are already present on TwirreShield)
#define SENS_VOLTAGE true
//sensor for measuring control inputs given by pilot. Enables NAZA actuator to perform manual command forwarding in autonomous mode (eg. auto control over yaw,gaz, with manual control over pitch,roll)
#define SENS_GR12 true

/*
 * Actuator configuration
 */
//PWM signal generation for controlling NAZA flight controller
#define ACT_NAZA true
//RGB status led. Due to a design issue of TwirreShield the led colours are only controlled binary
#define ACT_STATUSLED true
//80x60 mini OLED display attachable to TwirreShield
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
OLED * oled;
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
    sRFSonar = new SRFSonar("sonar1", SENS_SONAR_ADDR, SRF08);
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
    #if SENS_GR12
      naza = Naza::Initialize("naza", gR12);
    #else
      naza = Naza::Initialize("naza");
    #endif
    actuatorList.Add(naza);
  #endif

  #if ACT_OLED
    oled = new OLED("OLED");
    actuatorList.Add(oled);
  #endif
  
  #if ACT_STATUSLED 
    statusled = new StatusLED("RGB_LED");
    actuatorList.Add(statusled);    
    statusled->SetValue(0,255,0);
  #endif


  //pulse heartbeat led, and some delays for stabilisation
  delay(100);
  digitalWrite(HBLED, HIGH);
  delay(500);
  digitalWrite(HBLED, LOW);
  
  //clear the serial buffer
  while (SerialUSB.available())
  {
    SerialUSB.read();
  }

  //create request handler (will handle serial communication with host PC)
  requestHandler = new RequestHandler(&sensorList, &actuatorList, &SerialUSB);

  //final stabilisation delay
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
  if(ctr == HBLED_TOGGLE_COUNT)
  {
    ctr = 0;
    digitalWrite(HBLED, on);
    on = 1 - on; 
  }
}
