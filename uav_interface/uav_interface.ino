#include "RequestHandler.h"
#include "Sensor42.h"

RequestHandler* requestHandler;

Sensor42 sensor42("sensor42");

SensorList sensorList(1);

void setup()
{
    Serial.begin(115200);
    Stream* ser = &Serial;
    
    sensorList.elements[0] = &sensor42;
    
    requestHandler = new RequestHandler(&sensorList, ser);
}

void loop()
{
  requestHandler->SendAndReceive();
  sensor42.UpdateSensor();
} 
