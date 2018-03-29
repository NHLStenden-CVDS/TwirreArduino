#ifndef HEDGEHOG_H_
#define HEDGEHOG_H_

#include "Device.h"

#define HEDGEHOG_CS_PIN 52
#define HEDGEHOG_BUF_SIZE 32 
#define HEDGEHOG_PACKET_SIZE 29

class Hedgehog : public Device
{
  public:
    Hedgehog(const char *name);
    void init();
    void Update();
  private:
    void loop_hedgehog();
    void hedgehog_set_crc16(uint8_t *buf, uint8_t size);

  
    float _pos_x, _pos_y, _pos_z;
    uint32_t _pos_timestamp;
    uint8_t hedgehog_spi_buf[HEDGEHOG_BUF_SIZE];
    int8_t _lastfail;
    uint8_t _failbyte;
    int32_t _failcount;
    
    const int hedgehog_packet_header[5]= {0xff, 0x47, 0x01, 0x00, 0x10};
    typedef union {uint8_t b[2]; unsigned int w;} uni_8x2_16;

    unsigned long lastUpdate = 0;

    bool hedgehog_pos_updated;

};

#endif
