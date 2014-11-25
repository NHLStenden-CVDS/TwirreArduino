#include "RequestHandler.h"

RequestHandler()
{
    _currentMessage = 0;
    _messagesToSend = 0;

    //Initialize Serial communication
    _serial;
}

virtual ~RequestHandler(){}

void RequestHandler::SendAndReceive()
{
	if (_messagesToSend)
	{
		_SendMessage(_messageQueue[_currentMessage]);
		delete _messageStringQueue[_currentMessage];
		_messagesToSend--;
		_currentMessage++;
		if(_currentMessage == MAX_NR_MESSAGES)
			_currentMessage = 0;
	}
	else if (_serial.available())
	{
		_HandleRequest();
	}
}

void RequestHandler::_HandleRequest()
{
    uint8_t opcode;
    //read the first byte
    char* buffer;
    switch(opCode)
    {
        case 'P':
            //send a one-byte message with a 'P' back
            buffer = new buffer[1];
            buffer[1] = 'P';
            _SendMessage(buffer, 1);
            break;
        default:
            break;
    }
}

void RequestHandler::_AddToQueue(char* header, uint16_t headerSize, char* data, uint16_t dataSize)
{
    //If the total ammount of messages fit in the queue, we will put them there
    uint16_t nrMessages = size / MSG_MAX_SIZE + 1;
    if(nrMessages <= MAX_NR_MESSAGES - _messagesToSend)
    {
		for (uint16_t i = 0; i < nrMessages; i++)
            //Create message
            char*

            //Add message string to the queue
            uint8_t nextMessage = _currentMessage + _messagesToSend;
            if (nextMessage >= MAX_NR_MESSAGES)
                nextMessage -= MAX_NR_MESSAGES;
            uint8_t* mdata;
            m.GetString(mdata);
            _messageStringQueue[nextMessage] = mdata;

            dataSizeLeft -= messageDataSize;
        }
    }
}

void RequestHandler::_SendMessage(uint8_t* buffer, uint16_t size)
{
    //TODO: whole function ;)
    //would it be better to store the message size in another buffer
    //so we don't have to get it from the string???
}
