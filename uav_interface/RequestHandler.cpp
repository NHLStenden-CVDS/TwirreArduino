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
  switch (opCode)
  {
      //All the cases should add a message to the queue!
    case 'P':
      {
        //send a one-byte message with a 'P' back
        char buffer = 'P';
        _AddToQueue('P', nullptr, 0);
        break;
      }
    case 'I':
      //get sensor name, description and outputformat and put it like name|desc|format for every sensor. Separate these strings by ; and send it back.
      //Take care about memory (free) of the format but not the description or the name.
      break;
    case 'S':
      {
        uint8_t sensorID = _ReadSensorID();
        if (sensorID < _sensorList->length)
        {
          Payload payload = _ReadPayload();
          
          /*for(uint8_t i = 0; i < payload.size; i++)
          {
            SensorData sensorData = _sensorList->elements[sensorID]->GetValue((uint8_t)payload.buffer[i]); //get the ith value
          }*/
          SensorData sensorData = _sensorList->elements[sensorID]->GetValue((uint8_t)payload.buffer[0]); //let's do the first one for now
          _AddToQueue('O', sensorData.data, sensorData.size);
        }
        else
        {
          _Error("Incorrect sensor ID.");
        }
        break;
      }
    default :
      _Error("Unsupported operation code.");
  }

  //A request will always have a response:
  _SendNextMessage();
}

void RequestHandler::_AddToQueue(char opcode, char* buffer, uint16_t bufferSize)
{
  //Calculate in how many messages the (opcode + buffer) has to be fitted. bufferSize + 1 because of the opcode. MSG_MAX_SIZE + 1 because we want 64/64 to be 0. +1 to make it one message.
  uint16_t nrOfMessages = (bufferSize + 1) / (MSG_MAX_SIZE + 1) + 1;

  //If the total ammount of messages don't fit in the queue, we put an error message in the queue
  if (nrOfMessages > MAX_NR_MESSAGES)
  {
    _Error("[_AddToQueue] The buffer is too big.");
  }
  //else we split the buffer into nrOfMessages messages
  else
  {
    //bufferSize + 1 because of the opcode, -1 because of the formula
    uint16_t lastMessageSize = (bufferSize + 1) - ( (bufferSize + 1 - 1) / MSG_MAX_SIZE ) * MSG_MAX_SIZE;

    //_messageQueue will always be empty at this point
    _currentMessage = 0;

    for (uint16_t i = 0; i < nrOfMessages; i++)
    {
      uint16_t messageSize = (i == nrOfMessages - 1) ? lastMessageSize : MSG_MAX_SIZE;
      char* messageBuffer = new char[messageSize];
      // Copying the bytes of the current message from the buffer.
      if (i == 0)
      {
        *messageBuffer = opcode;
        if (bufferSize != 0)
        {
          memcpy(messageBuffer + 1, buffer, messageSize - 1);
        }
      }
      else
      {
        memcpy(messageBuffer, buffer + i * MSG_MAX_SIZE - 1, messageSize);
      }
      _messageQueue[i].buffer = std::unique_ptr<char>(messageBuffer);
      _messageQueue[i].size = messageSize;
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
  if (_messagesToSend)
  {
    _stream->write(_messageQueue[_currentMessage].buffer.get(), _messageQueue[_currentMessage].size);
    _messagesToSend--;
    _currentMessage++;
  }
}

inline void RequestHandler::_Error(char* errorMessage)
{
  _AddToQueue('E', errorMessage, strlen(errorMessage) + 1); //messageLength + 1 for the NULL terminator character
}
