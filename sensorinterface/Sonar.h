#ifndef SONAR_H_
#define SONAR_H_

class Sonar: public IntSensor {
public:
    void Init();
    int GetValue();
};

#endif
