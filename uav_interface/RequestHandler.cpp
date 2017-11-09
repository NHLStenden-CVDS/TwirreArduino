/*
 * Twirre: architecture for autonomous UAVs using interchangeable commodity components
 *
 * Copyright© 2017 Centre of expertise in Computer Vision & Data Science, NHL Stenden University of applied sciences
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "RequestHandler.h"
#include <cstring>
#include <cstdlib>

//#define DEBUG

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
  
  // TODO: every case to a sepparated handler!
  switch (opCode)
  {
      //All the cases should add a message to the queue!
    case 'P':
      {
        //send a one-byte message with a 'P' back
        _AddToQueue('P');
        break;
      }
    case 'I':
      {
        switch (_ReadOpcode()) //read the next byte of the message. Which is an S or an A depending on if you want the info string of the sensors or the actuators (IS or IA respectively)
        {
          case 'S':
            {
              Payload payload = _DeviceListInfo(_sensorList);
              _AddToQueue('O', payload);
              break;
            }
          case 'A':
            {
              Payload payload = _DeviceListInfo(_actuatorList);
              _AddToQueue('O', payload);
              break;
            }
          default:
            _Error("Incorrect device type identifier (it should be IA or IS).");
        }

        break;
      }
    case 'A':
      {
        uint8_t actuatorID = _ReadDeviceID();
        if ( !(actuatorID < _actuatorList->GetLength()) )
        {
           _Error("Incorrect actuator ID.");
           break;
        }
        Payload payload = _ReadPayload();
        if ( !(payload.size > 0 && payload.data != nullptr) )
        {
          _Error("Unexpected payload size.");
          break;
        }
        if ( !(_Actuate(_actuatorList->Get(actuatorID), payload)))
        {
           _Error("Incorrect value ID for this actuator.");
           break;
        }    
        _AddToQueue('O');
        break;
      }
    case 'S':
      {
        uint8_t sensorID = _ReadDeviceID();
        if ( !(sensorID < _sensorList->GetLength()))
        {
          _Error("Incorrect sensor ID.");
          break;
        }
        Payload payload = _ReadPayload();
        if ( !(payload.size > 0 && payload.data != nullptr))
        {
          char emsg[45];
          sprintf(emsg, "Unexpected payload size. size=%d", payload.size);
          _Error(emsg);
          break;
        }
        
        Payload sensorData = _CreateSenseResponse(_sensorList->Get(sensorID), payload);
        if ( !(sensorData.size > 0 && sensorData.data.get() != nullptr))
        {
          _Error("Incorrect value ID for this sensor.");
        }
        _AddToQueue('O', sensorData);
        break;
      }
    case 'E':
      _Error("Error test");
      break;
    default :
      _Error("Unsupported operation code.");
  }

  //A request will always have a response:
  _SendNextMessage();
}

Payload RequestHandler::_DeviceListInfo(DeviceList* deviceList)
{
  //get every sensor's name, description and outputformat and put it like name|desc|format. Separate these strings by ; and return it.
  char* infoString = (char*)malloc(1); //Initialize it with an empty string, we will use realloc to extend it later
  *infoString = 0; //We set it as an empty string so we will be able to use strcat
  uint16_t infoStringSize = 0;
  for (uint16_t i = 0; i < deviceList->GetLength(); ++i)
  {
    //calculate size to extend the string for this sensor
    const char* name = deviceList->Get(i)->GetName();
    const char* description = deviceList->Get(i)->GetDescription();
    std::unique_ptr<char> format = deviceList->Get(i)->GetVariablesFormatString();
    if (i > 0)
    {
      infoStringSize++; //if it is not the first one, we will separate the sensors with a semicolon
    }
    infoStringSize += strlen(name) + strlen(description) + strlen(format.get()) + 2; //+2 for | |

    //We make the buffer bigger
    infoString = (char*)realloc(infoString, infoStringSize + 1); //+1 for the Null terminator
    if (i > 0)
    {
      strcat(infoString, ";"); //if it is not the first one, we will separate the sensors with a semicolon
    }
    strcat(infoString, name);
    strcat(infoString, "|");
    strcat(infoString, description);
    strcat(infoString, "|");
    strcat(infoString, format.get());
  }
  return Payload(infoString, infoStringSize + 1); //+1 for the null terminator.
}

bool RequestHandler::_Actuate(Device* actuator, Payload &payload)
{
  uint16_t bytesToRead = payload.size;
  char* currentDataPosition = payload.data.get();

  while (bytesToRead > 0)
  {
    uint8_t variableID = *(uint8_t*)currentDataPosition;
    currentDataPosition++;
    bytesToRead--;
#ifdef DEBUG
    variableID = atoi((char*)&variableID);
#endif
    uint16_t bytesRead = actuator->SetVariable(variableID, currentDataPosition);
    bytesToRead -= bytesRead;
    currentDataPosition += bytesRead;
  }
  
  actuator->ValuesChanged();

  return (bytesToRead == 0);
}

Payload RequestHandler::_CreateSenseResponse(Device* sensor, Payload &request)
{ 
  sensor->OnRequest();
  
  uint8_t nrOfVariables = sensor->GetNumberOfVariables();

  //Get the size of the data
  uint32_t totalSize = 0;
  for (uint8_t i = 0; i < request.size; i++)
  {
    uint8_t variableID = (uint8_t)request.data.get()[i];
#ifdef DEBUG
    variableID = atoi((char*)&variableID);
#endif
    DeviceVariableSize variableSize = sensor->GetVariableSize(variableID);
    if (variableSize.numberOfElements > 0 && variableSize.elementSize > 0)
    {
      totalSize += variableSize.numberOfElements * variableSize.elementSize;
      if (variableSize.isArray)
      {
        //It is an array! we have to allocate the size as well!
        totalSize += sizeof(variableSize.numberOfElements);
      }
    }
    else
    {
      return Payload(nullptr, 0);
    }
  }

  //allocate the data
  char* data = (char*)malloc(totalSize);

  //we will use an auxiliar pointer to the data to copy all the values into their positions
  char* currentDataPosition = data;

  //put the values in the data
  for (uint8_t i = 0; i < request.size; i++)
  {
    uint8_t variableID = (uint8_t)request.data.get()[i];
#ifdef DEBUG
    variableID = atoi((char*)&variableID);
#endif

    DeviceVariable sensorVariable = sensor->GetVariable(variableID);
    //If it is an array, let's put the number of elements first
    if (sensorVariable.variableSize.isArray)
    {
      uint16_t size = sizeof(sensorVariable.variableSize.numberOfElements);
      memcpy(currentDataPosition, &(sensorVariable.variableSize.numberOfElements), size);
      currentDataPosition += size;
    }
    //we copy the value
    uint16_t size = sensorVariable.variableSize.numberOfElements * sensorVariable.variableSize.elementSize;
    memcpy(currentDataPosition, sensorVariable.variable, size);
    currentDataPosition += size;
  }

  return Payload(data, totalSize);
}

// Constructs one or more sendable messages from a payload and adds them to the sending-queue
// The first message will start with the opCode (O, E, etc.)
// The size of the payload won't be sent. If we want to do so, it has to be added to the first message like we do for the opCode.
void RequestHandler::_AddToQueue(char opCode, Payload &payload)
{
  // WARNING:s
  // This function will reset the queue every time it is used.
  // If you add another payload before the previous one (all the messages) has been sent, it will overwrite the previous one.


  //Calculate in how many messages the (opCode + data) has to be fitted. dataSize + 1 because of the opcode, but also - 1 because we want 64/64 to be 0. +1 to make it one message.
  uint16_t nrOfMessages = (payload.size + 1 - 1) / (MSG_MAX_SIZE) + 1;

  //If the total ammount of messages don't fit in the queue, we put an error message in the queue
  if (nrOfMessages > MAX_NR_MESSAGES)
  {
    _Error("[_AddToQueue] The payload is too big.");
  }
  //else we split the data into nrOfMessages messages
  else
  {
    //dataSize + 1 because of the opcode, -1 because of the formula
    uint16_t lastMessageSize = (payload.size + 1) - (nrOfMessages - 1) * MSG_MAX_SIZE;

    //_messageQueue will always be empty at this point
    _currentMessage = 0;

    for (uint16_t i = 0; i < nrOfMessages; i++)
    {
      uint16_t messageSize = (i == nrOfMessages - 1) ? lastMessageSize : MSG_MAX_SIZE;
      char* messageData = new char[messageSize];
      // Copying the bytes of the current message from the data.
      if (i == 0)
      {
        *messageData = opCode;
        if (payload.size != 0)
        {
          memcpy(messageData + 1, payload.data.get(), messageSize - 1);
        }
      }
      else
      {
        memcpy(messageData, payload.data.get() + i * MSG_MAX_SIZE - 1, messageSize);
      }
      _messageQueue[i] = Message(messageData, messageSize);
      _messagesToSend++;
    }
  }
}

inline void RequestHandler::_AddToQueue(char opCode)
{
  Payload payload(nullptr, 0);
  _AddToQueue(opCode, payload);
}

inline char RequestHandler::_ReadOpcode()
{
  char opcode = ' '; //some random opcode that we won't use, in case readBytes times out
  _stream->readBytes(&opcode, 1);
  return opcode;
}

Payload RequestHandler::_ReadPayload()
{
  uint16_t size = 0;
  if (_stream->readBytes((char*)&size, 2) == 2) //read the size of the payload, which is a uint16_t
  {
#ifdef DEBUG
    size = atoi((char*)&size);
#endif
    uint16_t expectedPayloadSize = size;
    if (size > 0)
    {
      char* data = new char[size];
      size = _stream->readBytes(data, size);
      if (size == expectedPayloadSize)
      {
        return Payload(data, size);
      }
      else
      {
        delete data;
      }
    }
  }
  return Payload(nullptr, size); //return if not successful
}

inline uint8_t RequestHandler::_ReadDeviceID()
{
  uint8_t id = 255; //We set it to a high value because if readBytes times out, it will return a bad ID. (Except if you have 256 devices, of course)
  _stream->readBytes(&id, 1);
#ifdef DEBUG
  id = atoi((char*)&id);
#endif
  return id;
}

//returns the number of bytes sent
uint16_t RequestHandler::_SendNextMessage()
{
  // make sure that the data in the tx buffer is not overwritten.
  // this call returns when all left over data in the tx buffer is sent.
  _stream->flush();
  
  if (_messagesToSend)
  {
    uint16_t bytesWriten = _stream->write(_messageQueue[_currentMessage].data.get(), _messageQueue[_currentMessage].size);
    _messagesToSend--;
    _currentMessage++;
    return bytesWriten;
  }
  else
  {
    return 0;
  }
}

inline void RequestHandler::_Error(const char* errorString)
{
  uint16_t size = strlen(errorString);
  char* errorMessage = new char[size + 1]; //+ 1 for the NULL terminator
  strcpy(errorMessage, errorString);
  Payload payload(errorMessage, size + 1); //messageLength + 1 for the NULL terminator character
  _AddToQueue('E', payload);
}
