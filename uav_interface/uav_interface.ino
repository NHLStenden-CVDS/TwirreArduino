#include "RequestHandler.h"
#include "Sensor42.h"

RequestHandler* requestHandler;

Sensor42 sensor42("sensor42");
Sensor42 secondSensor42("secondSensor42");

DeviceList sensorList;
DeviceList actuatorList;

void setup()
{
    Serial.begin(115200);
    Stream* ser = &Serial;
    
    sensorList.Add(&sensor42);
    sensorList.Add(&secondSensor42);
    
    requestHandler = new RequestHandler(&sensorList, &actuatorList, ser);
}

void loop()
{
  requestHandler->SendAndReceive();
  sensorList.UpdateAll();
} 
