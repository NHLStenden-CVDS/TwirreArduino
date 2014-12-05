#include "RequestHandler.h"
#include <cstring>

#define DEBUG

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
    {
      //get every sensor's name, description and outputformat and put it like name|desc|format. Separate these strings by ; and send it.
      //Take care about memory (free) of the format but not the description or the name.
      char* initString = (char*)malloc(1); //Initialize it with an empty string, we will use realloc to extend it later
      *initString = NULL; //We set it as an empty string so we will be able to use strcat
      uint16_t initStringSize = 0;
      for(uint16_t i = 0; i < _sensorList->length; ++i)
      {
        //calculate size to extend the string for this sensor
        char* name = _sensorList->elements[i]->GetSensorName();
        char* description = _sensorList->elements[i]->GetSensorDescription();
        char* outputFormat = _sensorList->elements[i]->GetOutputFormatString();
        initStringSize += strlen(name) + strlen(description) + strlen(outputFormat) + 2 + 1; //+2 for | | and + 1 for the ; that separates two sensors in the string
        initString = (char*)realloc(initString, initStringSize + 1); //+1 for the Null terminator
        if(i>0)
        {
          strcat(initString, ";"); //if it is not the first one, we will separate the sensors with a semicolon
        }
        strcat(initString, name);
        strcat(initString, "|");
        strcat(initString, description);
        strcat(initString, "|");
        strcat(initString, outputFormat);
        free(outputFormat); //we have to take care of this! not name or description because they are members of the Sensor class. The output string is generated when calling GetOutputFormatString().
      }
      _AddToQueue('O', initString, strlen(initString) + 1); //+1 for the null terminator
      break;
    }
    case 'S':
    {
      uint8_t sensorID = _ReadSensorID();
      if(sensorID < _sensorList->length)
      {
        Payload payload;
        if(_ReadPayload(payload))
        {
          if(payload.size > 0)
          {
            uint8_t nrOfValues = _sensorList->elements[sensorID]->GetNumberOfValues();
            /*for(uint8_t i = 0; i < payload.size; i++)
            {
              SensorData sensorData = _sensorList->elements[sensorID]->GetValue((uint8_t)payload.buffer[i]); //get the ith value
              .....
              ....
              ...*/
              uint8_t valueID = (uint8_t)payload.buffer.get()[0];
              #ifdef DEBUG
                valueID -= 48; //char to int8: '1' -> 1
              #endif
              if(valueID < nrOfValues)
              {
                Sensor::Data sensorData = _sensorList->elements[sensorID]->GetValue(valueID); //let's do the first one for now
                _AddToQueue('O', sensorData.data, sensorData.size);
              }
              else
              {
                _Error("Incorrect value ID for this sensor.");
              }
          }
          else
          {
            _Error("Payload size is 0.");
          }
        }
        else
        {
          _Error("Wrong payload size.");
        }
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

void RequestHandler::_AddToQueue(char opcode, void* buffer, uint16_t bufferSize)
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

bool RequestHandler::_ReadPayload(Payload &payload)
{
  bool everythingAllright = true;
  _stream->readBytes(&payload.size, 1);
  char* buffer;
  #ifdef DEBUG
    payload.size -= 48; //char to int8: '1' -> 1
  #endif
  if (payload.size == 0)
  {
    buffer = nullptr;
  }
  else
  {
    buffer = new char[payload.size];
    payload.size = _stream->readBytes(buffer, payload.size);
    everythingAllright = false;
  }
  payload.buffer = std::unique_ptr<char>(buffer);
  return everythingAllright;
}

inline uint8_t RequestHandler::_ReadSensorID()
{
  uint8_t sensorID;
  _stream->readBytes(&sensorID, 1);
  #ifdef DEBUG
    sensorID -= 48;
  #endif
  return sensorID;
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
