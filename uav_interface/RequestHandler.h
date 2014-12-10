#ifndef REQUEST_H_
#define REQUEST_H_

#include <Stream.h>
#include "Sensor.h"
#include <memory>
#include "SensorList.h"

#define MAX_NR_MESSAGES 1025
#define MSG_MAX_SIZE 64 //If this is changed to more than 255, in the protocol, change payloadSize to more than one byte (uint16_t?)

struct Message
{
    std::unique_ptr<char> buffer;
    uint16_t size;
};
	
struct Payload
{
    std::unique_ptr<char> buffer;
    uint8_t size;
};

struct SensorData
{
  std::unique_ptr<void> data;
  uint16_t size;
};


class RequestHandler
{
public:
    RequestHandler(SensorList* sensorList, Stream* stream);
    void SendAndReceive();
    void Initialize();

private: 
    void _HandleRequest();
    void _AddToQueue(char opcode, void* buffer, uint16_t bufferSize);
    uint16_t _SendNextMessage();
    inline void _Error(char* errorMessage);
    inline char _ReadOpcode();
    inline uint8_t _ReadSensorID();
    bool _ReadPayload(Payload &payload);
    SensorData _ConstructSensorData(Sensor* sensor, Payload &payload);
    
    uint16_t _currentMessage;
    uint16_t _messagesToSend;
    
    SensorList* _sensorList;
    
    Message _messageQueue[MAX_NR_MESSAGES];
    Stream* _stream;
};

#endif
