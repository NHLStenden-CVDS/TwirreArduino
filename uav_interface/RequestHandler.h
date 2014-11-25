#ifndef REQUEST_H_
#define REQUEST_H_

#define MAX_NR_MESSAGES 40
#define MSG_MAX_SIZE 1024

class RequestHandler {
public:
    RequestHandler();
    void SendAndReceive();
    void Initialize();

private:
    void _HandleRequest();
    void _AddToQueue(char* header, uint16_t headerSize, char* data, uint16_t dataSize);
    void _SendMessage(uint8_t* buffer, uint16_t size);
    
    uint16_t _currentMessage;
    uint16_t _messagesToSend;
	char* _messageQueue[MAX_NR_MESSAGES];
    _serial;
};

#endif
