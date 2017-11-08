 /* 
  * Copyright © 2017, Centre of Expertise Computer Vision & Data Science, NHL Stenden university of applied sciences
  * All rights reserved.
  * 
  * no warranty, no liability
  */

#ifndef DEVICELIST_H_
#define DEVICELIST_H_

#include "Device.h"

class DeviceList
{
  public:
    DeviceList();
    ~DeviceList();

    /**
     * Adds a device to the list.
     */
    void Add(Device* device);
    
    /**
     * Give the length of the list
     * @return Returns _length, which is a uint8_t.
     */
    uint8_t GetLength();
    
    /**
     * Gives a pointer to the device specified with id.
     * @return Returns a Device pointer.
     */
    Device* Get(uint8_t id);
    
    /**
     * Calls the Update() method of every device in the list.
     */
    void UpdateAll();

  private:
    uint8_t _length;
    Device** _elements;
};

#endif
