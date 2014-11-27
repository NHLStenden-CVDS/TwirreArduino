#include "Stream.h"
#include "Sensor.h"

#ifndef REQUEST_H_
#define REQUEST_H_

#define MAX_NR_MESSAGES 40
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


class RequestHandler
{
public:
    RequestHandler(SensorList* sensorList, Stream* stream);
    void SendAndReceive();
    void Initialize();

private:
    char* incommingBuffer;

    void _HandleRequest();
    void _AddToQueue(char* buffer, uint16_t bufferSize);
    void _SendNextMessage();
    void _Error(char* errorMessage);
    inline char _ReadOpcode();
    inline uint8_t _ReadSensorID();
    Payload _ReadPayload();
    
    uint16_t _currentMessage;
    uint16_t _messagesToSend;
    
	SensorList* _sensorList;
    
	Message _messageQueue[MAX_NR_MESSAGES];
    Stream* _stream;
};

#endif
