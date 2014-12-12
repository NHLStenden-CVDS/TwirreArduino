#ifndef REQUEST_H_
#define REQUEST_H_

#include <Stream.h>
#include "Device.h"
#include "DeviceList.h"

#define MAX_NR_MESSAGES 1025
#define MSG_MAX_SIZE 64 //If this is changed to more than 255, in the protocol, change payloadSize to more than one byte (uint16_t?)



struct Payload
{
  std::unique_ptr<char> data;
  uint16_t size;

  Payload() : data(nullptr), size(0) {}
  Payload(void* data, uint16_t size) : data((char*)data), size(size) {}
};

typedef Payload Message;
typedef Payload SensorData;

class RequestHandler
{
  public:
    RequestHandler(DeviceList* deviceList, DeviceList* actuatorList, Stream* stream);
    void SendAndReceive();
    void Initialize();

  private:
    void _HandleRequest();
    void _AddToQueue(char opCode, Payload &payload);
    inline void _AddToQueue(char opCode);
    uint16_t _SendNextMessage();
    inline void _Error(char* errorMessage);
    inline char _ReadOpcode();
    inline uint8_t _ReadDeviceID();
    Payload _ReadPayload();
    Payload _CreateSenseResponse(Device* sensor, Payload &request);
    bool _Actuate(Device* sensor, Payload &payload);
    Payload _DevicesInfoPayload(DeviceList* deviceList);

    uint16_t _currentMessage;
    uint16_t _messagesToSend;

    DeviceList* _sensorList;
    DeviceList* _actuatorList;

    Message _messageQueue[MAX_NR_MESSAGES];
    Stream* _stream;
};

#endif
