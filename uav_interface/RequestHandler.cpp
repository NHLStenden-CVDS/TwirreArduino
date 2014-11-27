#include "RequestHandler.h"
#include "Sensor.h"
#include <Stream.h>
#include <cstring>

RequestHandler::RequestHandler(SensorList* sensorList, Stream* stream)
{
    _currentMessage = 0;
    _messagesToSend = 0;

	_sensorList = sensorList;
    _stream = stream;
}

void RequestHandler::SendAndReceive()
{
	if (_messagesToSend)
	{
		//If a previous request had a long message, we still handle the remaining messages
		_SendNextMessage();
	}
	else if (_stream->available())
	{
		_HandleRequest();
	}
}

void RequestHandler::_HandleRequest()
{
    char opCode = _ReadOpcode();
    switch(opCode)
    {
        case 'P':
        {
            //send a one-byte message with a 'P' back
            char buffer = 'P';
            _AddToQueue(&buffer, 1);
            break;
        }
        case 'S':
        {
	  uint8_t sensorID = _ReadSensorID();
          if (sensorID < _sensorList->length)
          {
            Payload payload = _ReadPayload();
            SensorData sensorData = _sensorList->elements[sensorID]->Output(payload.size, payload.buffer.get());
            _AddToQueue(sensorData.data.get(), sensorData.size);
          }
          else
          {
            _Error("Incorrect sensor ID.");
          }
          break;
        }
        default :
          _Error("Unsupported operation code");
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
            _Error("Sensor data is too big.");
	}
	//else we split the buffer into nrOfMessages messages
    else
    {
		uint16_t lastMessageSize = bufferSize % MSG_MAX_SIZE;
		
		//_messageQueue will always be empty at this point
		_currentMessage = 0;
		
		for (uint16_t i = 0; i < nrOfMessages; i++)
		{
            uint16_t size = (i == nrOfMessages - 1) ? lastMessageSize : MSG_MAX_SIZE;
            char* messageBuffer = new char[size];
            // Copying the bytes of the current message from the buffer.
			memcpy(messageBuffer, buffer + i * MSG_MAX_SIZE, size);
			
			_messageQueue[i].buffer = std::unique_ptr<char>(messageBuffer);
			_messageQueue[i].size = size;
			_messagesToSend++;
        }
    }
}

inline char RequestHandler::_ReadOpcode()
{
	char opcode;
	_stream->readBytes(&opcode, 1);
	return opcode;
}

Payload RequestHandler::_ReadPayload()
{
	Payload payload;
	_stream->readBytes(&payload.size, 1);
	char* buffer;
        payload.size -= 48;
        if (payload.size == 0)
        {
            buffer = nullptr;
        }
        else
        {
            buffer = new char[payload.size];
            _stream->readBytes(buffer, payload.size);
        }
	payload.buffer = std::unique_ptr<char>(buffer);
	return payload;
}

inline uint8_t RequestHandler::_ReadSensorID()
{
	uint8_t sensorID;
	_stream->readBytes(&sensorID, 1);
	return sensorID - 48;
}

void RequestHandler::_SendNextMessage()
{
        if(_messagesToSend)
        {
    	    _stream->write(_messageQueue[_currentMessage].buffer.get(), _messageQueue[_currentMessage].size);
    	    _messagesToSend--;
    	    _currentMessage++;
        }
}

void RequestHandler::_Error(char* errorMessage)
{
  int messageLength = strlen(errorMessage);
  char* buffer = new char[messageLength + 2]; //+2: 1 for the E and 1 for the NULL terminator character
  *buffer = 'E';
  memcpy(buffer + 1, errorMessage, messageLength + 1); //messageLength + 1 for the NULL terminator character
  _AddToQueue(buffer, messageLength + 2);
  delete buffer;
}
