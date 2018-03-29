#include "Hedgehog.h"
#include <SPI.h>

Hedgehog::Hedgehog(const char* name) : Device(name, "Hedgehog, ultrasonic position tracking system")
{
  _AddVariable("pos_x", &_pos_x);
  _AddVariable("pos_y", &_pos_y);
  _AddVariable("pos_z", &_pos_z);
  _AddVariable("pos_time", &_pos_timestamp);
  _AddVariable("lastfail", &_lastfail);
  _AddVariable("failcount", &_failcount);
  _AddVariable("failbyte", &_failbyte);
  init();
}

void Hedgehog::init(){
  _pos_x = 0;
  _pos_y = 0;
  _pos_z = 0;
  _pos_timestamp = 0;
  _failcount = 0;
  _lastfail = -1;
  // start the SPI library
  pinMode(53, OUTPUT);
  digitalWrite(53, HIGH);
  SPI.begin();
  pinMode(HEDGEHOG_CS_PIN,OUTPUT);// set chip select for hedgehog  
  digitalWrite(HEDGEHOG_CS_PIN, HIGH);
  hedgehog_pos_updated = false;
  SPI.setClockDivider(21);
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
}

void Hedgehog::Update()
{
  unsigned long curTime = micros();

  if(curTime>lastUpdate + 10000){  //Sensor updated aprox 100 times a second
    lastUpdate = curTime;
    loop_hedgehog();
  }
}

// Marvelmind hedgehog service loop
void Hedgehog::loop_hedgehog(){
  int incoming_byte;
  int i;
  int packet_received;
  packet_received = true;
  
  digitalWrite(HEDGEHOG_CS_PIN, LOW);// hedgehog chip select 
  
  delayMicroseconds(50);
  for(i=0;i<HEDGEHOG_PACKET_SIZE;i++){
    incoming_byte= SPI.transfer(0x00);// read byte from hedgehog  
    //Serial.println(incoming_byte);
    hedgehog_spi_buf[i]= incoming_byte;  
  }

  delayMicroseconds(50);
  digitalWrite(HEDGEHOG_CS_PIN, HIGH);// hedgehog chip unselect  

  packet_received = false;
  _lastfail = -1;
  //check header
  if(hedgehog_spi_buf[0] != 0xff || hedgehog_spi_buf[1] != 0x47)
  {
    _lastfail = 1;  // header error
    _failbyte = hedgehog_spi_buf[1];
    _failcount++;
  }
  else if(!(hedgehog_spi_buf[2] == 0x01 || hedgehog_spi_buf[2] == 0x11) || hedgehog_spi_buf[3] != 0x00)
  {
    _lastfail = 2;  // unrecognized packet type
    _failbyte = hedgehog_spi_buf[2];
    _failcount++;
  }
  else
  {
    packet_received = true;
  }
   
  if (packet_received){
    if(hedgehog_spi_buf[4] == 0x10)
    {
      //old cm-resolution packet
      hedgehog_set_crc16(&hedgehog_spi_buf[0], 23);// calculate CRC checksum of packet
      if ((hedgehog_spi_buf[23] == 0)&&(hedgehog_spi_buf[24] == 0)){// checksum success
        _pos_x= static_cast<float>(int(hedgehog_spi_buf[9]) | (int(hedgehog_spi_buf[10])<<8)) / 100.0f;
        _pos_y= static_cast<float>(int(hedgehog_spi_buf[11]) | (int(hedgehog_spi_buf[12])<<8)) / 100.0f; // coordinates of hedgehog (X,Y), cm
        _pos_z= static_cast<float>(int(hedgehog_spi_buf[13]) | (int(hedgehog_spi_buf[14])<<8)) / 100.0f; // height of hedgehog, cm (FW V3.97+)
        hedgehog_pos_updated= true;// flag of new data from hedgehog received
        _pos_timestamp = micros();
      }
      else
      {
        _lastfail = 4;  // crc error
        _failcount++;
      }
    }
    else if(hedgehog_spi_buf[4] == 0x16)
    {
      //new mm-resolution packet
      hedgehog_set_crc16(&hedgehog_spi_buf[0], 29);// calculate CRC checksum of packet
      if ((hedgehog_spi_buf[29] == 0)&&(hedgehog_spi_buf[30] == 0)){// checksum success
        _pos_x= static_cast<float>(int(hedgehog_spi_buf[9]) | (int(hedgehog_spi_buf[10])<<8) | (int(hedgehog_spi_buf[11])<<16) | (int(hedgehog_spi_buf[12])<<24)) / 1000.0f;
        _pos_y= static_cast<float>(int(hedgehog_spi_buf[13]) | (int(hedgehog_spi_buf[14])<<8) | (int(hedgehog_spi_buf[15])<<16) | (int(hedgehog_spi_buf[16])<<24)) / 1000.0f;
        _pos_z= static_cast<float>(int(hedgehog_spi_buf[17]) | (int(hedgehog_spi_buf[18])<<8) | (int(hedgehog_spi_buf[19])<<16) | (int(hedgehog_spi_buf[20])<<24)) / 1000.0f;
        hedgehog_pos_updated= true;// flag of new data from hedgehog received
        _pos_timestamp = micros();
      }
      else
      {
        _lastfail = 4;  // crc error
        _failcount++;
      }
    }
    else
    {
      _lastfail = 3;  // payload size error
      _failbyte = hedgehog_spi_buf[4];
      _failcount++;
    }
  }
}

// Calculate CRC-16 of hedgehog packet
void Hedgehog::hedgehog_set_crc16(uint8_t *buf, uint8_t size) {
  uni_8x2_16 sum;
  uint8_t shift_cnt;
  uint8_t byte_cnt;
  sum.w=0xffffU;
  for(byte_cnt=size; byte_cnt>0; byte_cnt--){
    sum.w=(unsigned int) ((sum.w/256U)*256U + ((sum.w%256U)^(buf[size-byte_cnt])));
    for(shift_cnt=0; shift_cnt<8; shift_cnt++){
      if((sum.w&0x1)==1) sum.w=(unsigned int)((sum.w>>1)^0xa001U);
      else sum.w>>=1;
    }
  }
  buf[size]=sum.b[0];
  buf[size+1]=sum.b[1];// little endian
}
