#ifndef REQUEST_H_
#define REQUEST_H_

#include <Stream.h>
#include "Device.h"
#include "DeviceList.h"

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
    RequestHandler(DeviceList* deviceList, DeviceList* actuatorList, Stream* stream);
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
    SensorData _ConstructSensorData(Device* sensor, Payload &payload);
    std::unique_ptr<char> _GenerateDevicesInfoString();
    
    uint16_t _currentMessage;
    uint16_t _messagesToSend;
    
    DeviceList* _sensorList;
    DeviceList* _actuatorList;
    
    Message _messageQueue[MAX_NR_MESSAGES];
    Stream* _stream;
};

#endif
