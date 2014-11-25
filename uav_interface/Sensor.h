#ifndef SENSOR_H_
#define SENSOR_H_

class Sensor {
public:
    Sensor(std::string name);
    virtual uint16_t Output(uint8_t nrParam, char* param, char* data) = 0;
    virtual void InitializeSensor() = 0;
    virtual void UpdateSensor() = 0;

    std::string sensorName;
    std::string outputFormat;
};

#endif
