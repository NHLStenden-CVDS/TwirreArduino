#include Stream.h

#ifndef REQUEST_H_
#define REQUEST_H_

#define MAX_NR_MESSAGES 40
#define MSG_MAX_SIZE 64

class RequestHandler {
public:
    RequestHandler(Stream* stream);
    void SendAndReceive();
    void Initialize();

private:
    typedef struct m
    {
		uint16_t size;
		char* buffer;
	}Message;
	
	char* incommingBuffer;

    void _HandleRequest();
    void _AddToQueue(char* buffer, uint16_t bufferSize);
    void _SendNextMessage();
    
    uint16_t _currentMessage;
    uint16_t _messagesToSend;
    

    
	Message _messageQueue[MAX_NR_MESSAGES];
    Stream* _stream;
};

#endif
