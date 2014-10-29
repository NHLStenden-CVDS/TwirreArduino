#ifndef SENSOR_H_
#define SENSOR_H_

class Sensor {
public:
    virtual void GetData() = 0;
    virtual void Init() = 0;
    virtual void Update() = 0;

protected:
    int _id;
    const char* _name;
    int _pin;

};

#endif
