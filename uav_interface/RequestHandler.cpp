#include "RequestHandler.h"
#include <cstring>

#define DEBUG

RequestHandler::RequestHandler(DeviceList* sensorList, DeviceList* actuatorList, Stream* stream)
{
  _currentMessage = 0;
  _messagesToSend = 0;

  _sensorList = sensorList;
  _actuatorList = actuatorList;
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
      std::unique_ptr<char> sensorInfoStringSize = _GenerateDevicesInfoString();
      _AddToQueue('O', sensorInfoStringSize.get(), strlen(sensorInfoStringSize.get()) + 1); //+1 for the null terminator
      break;
    }
    case 'S':
    {
      uint8_t sensorID = _ReadSensorID();
      if(sensorID < _sensorList->GetLength())
      {
        Payload payload;
        if(_ReadPayload(payload))
        {
          if(payload.size > 0)
          {
            SensorData sensorData = _ConstructSensorData(_sensorList->Get(sensorID), payload);
            if(sensorData.size > 0 && sensorData.data.get() != nullptr)
            {
              _AddToQueue('O', sensorData.data.get(), sensorData.size);
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
          _Error("Error reading payload. Wrong payload size?");
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

std::unique_ptr<char> RequestHandler::_GenerateDevicesInfoString()
{
    //get every sensor's name, description and outputformat and put it like name|desc|format. Separate these strings by ; and send it.
    char* infoString = (char*)malloc(1); //Initialize it with an empty string, we will use realloc to extend it later
    *infoString = NULL; //We set it as an empty string so we will be able to use strcat
    uint16_t infoStringSize = 0;
    for(uint16_t i = 0; i < _sensorList->GetLength(); ++i)
    {
      //calculate size to extend the string for this sensor
      char* name = _sensorList->Get(i)->GetName();
      char* description = _sensorList->Get(i)->GetDescription();
      std::unique_ptr<char> format = _sensorList->Get(i)->GetVariablesFormatString();
      infoStringSize += strlen(name) + strlen(description) + strlen(format.get()) + 2 + 1; //+2 for | | and + 1 for the ; that separates two sensors in the string
      infoString = (char*)realloc(infoString, infoStringSize + 1); //+1 for the Null terminator
      if(i>0)
      {
        strcat(infoString, ";"); //if it is not the first one, we will separate the sensors with a semicolon
      }
      strcat(infoString, name);
      strcat(infoString, "|");
      strcat(infoString, description);
      strcat(infoString, "|");
      strcat(infoString, format.get());
    }
    return std::unique_ptr<char>(infoString);
}

SensorData RequestHandler::_ConstructSensorData(Device* sensor, Payload &payload)
{
  SensorData sensorData;
  uint8_t nrOfVariables = sensor->GetNumberOfVariables();
  
  //Get the size of the buffer
  uint16_t totalSize = 0;
  for(uint8_t i = 0; i < payload.size; i++)
  {
    uint8_t variableID = (uint8_t)payload.buffer.get()[i];
    #ifdef DEBUG
      variableID -= 48;
    #endif
    DeviceVariableSize variableSize = sensor->GetVariableSize(variableID);
    if(variableSize.numberOfElements > 0 && variableSize.elementSize > 0)
    {
      totalSize += variableSize.numberOfElements * variableSize.elementSize;
      if(variableSize.isArray)
      {
        //It is an array! we have to allocate the size as well!
        totalSize += sizeof(variableSize.elementSize);
      }
    }
    else
    {
      sensorData.size = 0;
      sensorData.data = std::unique_ptr<void>(nullptr);
      return sensorData;      
    }
  }
  
  //allocate the buffer
  void* data = malloc(totalSize);
  sensorData.data = std::unique_ptr<void>(data);
  sensorData.size = totalSize;
  
  //we will use an auxiliar pointer to the buffer to copy all the values into their positions
  void* currentBufferPosition = data;
  
  //put the values in the buffer
  for(uint8_t i = 0; i < payload.size; i++)
  {
    uint8_t variableID = (uint8_t)payload.buffer.get()[i];
    #ifdef DEBUG
      variableID -= 48;
    #endif
    
    DeviceVariable sensorVariable = sensor->GetVariable(variableID);
    //If it is an array, let's put the number of elements first
    if(sensorVariable.variableSize.isArray)
    {
        uint16_t size = sizeof(sensorVariable.variableSize.numberOfElements);
        memcpy(currentBufferPosition, &(sensorVariable.variableSize.numberOfElements), size);
        currentBufferPosition += size;
    }
    //we copy the value
    uint16_t size = sensorVariable.variableSize.numberOfElements * sensorVariable.variableSize.elementSize;
    memcpy(currentBufferPosition, sensorVariable.variable, size);
    currentBufferPosition += size;
  }
  
  return sensorData;
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
  char* buffer = nullptr;
  payload.size = 0;
  uint8_t receivedPayloadSize;
  
  if(everythingAllright = (_stream->readBytes(&payload.size, 1) == 1))
  {
    #ifdef DEBUG
      payload.size -= 48; //char to int8: '1' -> 1
    #endif
    receivedPayloadSize = payload.size;
    if (payload.size > 0)
    {
      buffer = new char[payload.size];
      payload.size = _stream->readBytes(buffer, payload.size);
      if(payload.size < receivedPayloadSize)
      {
        everythingAllright = false;
      }
    }
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

//returns the number of bytes sent
uint16_t RequestHandler::_SendNextMessage()
{
  if (_messagesToSend)
  {
    uint16_t bytesWriten = _stream->write(_messageQueue[_currentMessage].buffer.get(), _messageQueue[_currentMessage].size);
    _messagesToSend--;
    _currentMessage++;
    return bytesWriten;
  }
  else
  {
    return 0;
  }
}

inline void RequestHandler::_Error(char* errorMessage)
{
  _AddToQueue('E', errorMessage, strlen(errorMessage) + 1); //messageLength + 1 for the NULL terminator character
}
