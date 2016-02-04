#ifndef REQUEST_H_
#define REQUEST_H_

#include <Stream.h>
#include "Device.h"
#include "DeviceList.h"

#define MAX_NR_MESSAGES 1025
#define MSG_MAX_SIZE 128 //If this is changed to more than 255, in the protocol, change payloadSize to more than one byte (uint16_t?)



struct Payload
{
  std::unique_ptr<char> data;
  uint16_t size;

  Payload() : data(nullptr), size(0) {}
  Payload(void* data, uint16_t size) : data((char*)data), size(size) {}
};

typedef Payload Message;
typedef Payload SensorData;

class RequestHandler
{
  public:
    RequestHandler(DeviceList* deviceList, DeviceList* actuatorList, Stream* stream);
    
    /**
     * If there is a message in the queue, it sends it. Otherwise, if there is a request, it handles it.
     * To do so, it uses functions _SendNextMessage() and _HandleRequest().
     */
    void SendAndReceive();

  private:
    /**
     * This method handles a request from the main board. It reads the opcode, handles the request and generates a response.
     */
    void _HandleRequest();
    
    /**
     * This method splits the data of payload into messages of MSG_MAX_SIZE size. 
     * It adds the operation code at the beginning of the first message.
     * It adds then these messages to the queue and increments _messagesToSend.
     */
    void _AddToQueue(char opCode, Payload &payload);
    
    /**
     * This method calls _AddToQueue with the opCode provided and an empty payload.
     */ 
    inline void _AddToQueue(char opCode);
    
    /**
     * This method sends the next message (_currentMessage) of the queue (_messageQueue).
     * @return It returns the number of bytes sent.
     */ 
    uint16_t _SendNextMessage();
    
    /**
     * This method calls _AddToQueue with the opcode 'E' and a payload containing the errorMessage. 
     * errorMessage is a null terminated string that describes the error.
     */
    inline void _Error(const char* errorMessage);
    
    /**
     * This method reads one byte. 
     * @return Retuns the operation code as a char.
     */
    inline char _ReadOpcode();
    
    /**
     * This method reads one byte. 
     * @return Retuns the device id as a uint8_t.
     */
    inline uint8_t _ReadDeviceID();
    
    /**
     * Reads the first two bytes as the data size and the data. It puts them into a Payload object.
     * It will give an empty Payload if there is an error (like reading less bytes as the data size read -read times out-)
     * @return Payload object with the data and its size.
     */
    Payload _ReadPayload();
    
    /**
     * Parses the sense request. For every variable ID, it gets the value of the variable from the sensor.
     * It puts everything together in one payload and returns it.
     * @return Payload object with the data that has to be sent to the main board.
     */
    Payload _CreateSenseResponse(Device* sensor, Payload &request);
    
    /**
     * Parses the actuate request. For every variable ID, it sets the value of the variable to the actuator.
     * @return Returns a boolean that indicates success.
     */
    bool _Actuate(Device* actuator, Payload &payload);
    
    /**
     * For every device in the list, it gets the name, the description and the variable format and puts it into a string.
     * @return Returns the payload with the info string.
     */
    Payload _DeviceListInfo(DeviceList* deviceList);

    uint16_t _currentMessage;
    uint16_t _messagesToSend;

    DeviceList* _sensorList;
    DeviceList* _actuatorList;

    Message _messageQueue[MAX_NR_MESSAGES];
    Stream* _stream;
};

#endif
