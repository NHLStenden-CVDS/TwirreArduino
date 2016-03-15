#include "Hedgehog.h"
#include <arduino.h>
#include <SPI.h>

Hedgehog::Hedgehog(const char* name) : Device(name, "Hedgehog, ultrasonic position tracking system")
{
  _AddVariable("pos_x", &_pos_x);
  _AddVariable("pos_y", &_pos_y);
  _AddVariable("pos_z", &_pos_z);
  init();
}

void Hedgehog::init(){
  // start the SPI library
  SPI.begin();
  pinMode(HEDGEHOG_CS_PIN,OUTPUT);// set chip select for hedgehog  
  digitalWrite(HEDGEHOG_CS_PIN, HIGH);
  hedgehog_pos_updated = false;
  SPI.setClockDivider(42);
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
}

void Hedgehog::Update()
{
  unsigned long curTime = micros();

  if(curTime>lastUpdate + 100000){  //Sensor updated aprox 8 times a second
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
    
    // check first 5 bytes for constant value
    if (i<5)
    {
      if (incoming_byte != hedgehog_packet_header[i]) 
      {
        packet_received= false;// packet header error
      }
    }     
  }
  
  delayMicroseconds(50);
  digitalWrite(HEDGEHOG_CS_PIN, HIGH);// hedgehog chip unselect   
  if (packet_received){
    hedgehog_set_crc16(&hedgehog_spi_buf[0], HEDGEHOG_PACKET_SIZE);// calculate CRC checksum of packet
    if ((hedgehog_spi_buf[HEDGEHOG_PACKET_SIZE] == 0)&&(hedgehog_spi_buf[HEDGEHOG_PACKET_SIZE+1] == 0)){// checksum success
      _pos_x= int(hedgehog_spi_buf[9]) + (int(hedgehog_spi_buf[10])<<8);
      _pos_y= int(hedgehog_spi_buf[11]) + (int(hedgehog_spi_buf[12])<<8);// coordinates of hedgehog (X,Y), cm
      _pos_z= int(hedgehog_spi_buf[13]) + (int(hedgehog_spi_buf[14])<<8);// height of hedgehog, cm (FW V3.97+)
      hedgehog_pos_updated= true;// flag of new data from hedgehog received
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
