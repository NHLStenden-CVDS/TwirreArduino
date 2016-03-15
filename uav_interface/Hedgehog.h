#ifndef HEDGEHOG_H_
#define HEDGEHOG_H_

#include "Device.h"



class Hedgehog : public Device
{
  public:
    Hedgehog(const char *name);
    void Update();
    void OnRequest();

  private:
    int16_t _distance;
};

#endif
