#include "RequestHandler.h"

RequestHandler(Stream* stream)
{
    _currentMessage = 0;
    _messagesToSend = 0;

    _stream = stream;
}

virtual ~RequestHandler(){}

void RequestHandler::SendAndReceive()
{
	if (_messagesToSend)
	{
		//If a previous request had a long message, we still handle the remaining messages
		_SendNextMessage();
	}
	else if (_stream.available())
	{
		_HandleRequest();
	}
}

void RequestHandler::_HandleRequest()
{
    char opcode = _ReadOpcode();
    switch(opCode)
    {
        case 'P':
            //send a one-byte message with a 'P' back
            char buffer = 'P';
            _AddToQueue(&buffer, 1);
            break;
        default:
            break;
    }
    
    //A request will always have a response:
    _SendNextMessage();
}

void RequestHandler::_AddToQueue(char* buffer, uint16_t bufferSize)
{
	//Calculate in how many messages the buffer has to be fitted
    uint16_t nrOfMessages = bufferSize / (MSG_MAX_SIZE + 1) + 1;

    //If the total ammount of messages don't fit in the queue, we put an error message in the queue
	if(nrOfMessages > MAX_NR_MESSAGES)
	{
		_messageQueue[0] = new char[1];
		_messageQueue[0] = 'E';
	}
	//else we split the buffer into nrOfMessages messages
    else
    {
		uint16_t lastMessageSize = buffersize % MSG_MAX_SIZE;
		
		//_messageQueue will always be empty at this point
		_currentMessage = 0;
		
		for (uint16_t i = 0; i < nrOfMessages; i++)
		{
            uint16_t size = (i == nrOfMessages - 1) ? lastMessageSize : MSG_MAX_SIZE;
            _messageQueue[i].buffer = new char[size];
            
            // Copying the bytes of the current message from the buffer.
			memcpy(_messageQueue[i].buffer, _buffer + i * MSG_MAX_SIZE, size);
			_messageQueue[i].size = size;
			_messagesToSend++;
        }
    }
}

inline char RequestHandler::_ReadOpcode()
{
	char opcode;
	_stream.readBytes(opcode, 1);
	return opcode;
}

inline void RequestHandler::_SendNextMessage()
{
	_stream.write(_messageQueue[_currentMessage].buffer, _messageQueue[_currentMessage].size);
	delete message.buffer;
	_messagesToSend--;
	_currentMessage++;
}
