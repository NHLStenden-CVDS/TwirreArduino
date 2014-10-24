// Function for reading and handeling incoming messages
void ReadMessage()
{
  if (Serial.available() > 0) 
  {
    // Read incoming bytes to buffer
    Serial.readBytes((char *)incomingBuffer, PACKET_SIZE);
    
    // Check the checksum
    if(CheckChecksum())
    {
      // Handle the message
      HandleMessage();
    }  
  }
}

// Function for checking the message checksum
boolean CheckChecksum()
{
  byte checksum = 0;
  
  for(int i = 0; i < PACKET_SIZE -1; i++)
  {
    checksum += incomingBuffer[i];
  }

  return (checksum == incomingBuffer[15]) ? true : false;
}

// Function that sets the checksum
void CalculateChecksum(unsigned char * buffer)
{
  byte checksum = 0;
  
  for(int i = 0; i < PACKET_SIZE -1; i++)
  {
    checksum += buffer[i];
  }
  
  buffer[PACKET_SIZE -1] = checksum;
}

// Function for handling the specific messages
void HandleMessage()
{
  // Check the identifier byte and execute the corresponding function
  switch (incomingBuffer[0]) {
    case 'C':
      HandleControlMessage();
      break;
    case 'N':
      HandleSonarRequest();
      break;
    case 'T':
      HandleTestMessage();
      break;
    case 'A':
      HandleIsAutnomousMessage();
      break;
    case 'S':
      HandleStickInput();
      break;  
    case 'G':
      HandleGyroAngleRequest();
      break;
  }
}

// Function for sending the outgoing message
void SendOutgoingMessage()
{
  CalculateChecksum(outgoingBuffer);
  
  Serial.write(outgoingBuffer, PACKET_SIZE);
}

///////////////////////////////////////////////////
// Specific incoming message handling functions //
///////////////////////////////////////////////////

// Function which handles the control message
void HandleControlMessage()
{
  // Map the message as a controlData struct
  controlData* data = (controlData*) &incomingBuffer[1];
  
  ControlMode = data->mode;
  TC4H = ControlMode>>8;
  OCR4D = (ControlMode&0xFF);
  
  //check if a specific timeout is specified. Otherwise continue the current timeout procedure.
  if(data->timeout > 0)
  {
    control_timeout = data->timeout; 
  }
  else if(control_timeout == 0)
  {
    //if timeout already occured, do not set the controls.
     return;
  }
 
  // Setting new PWM signals
  OCR3A = data->yaw;    //D5
  OCR1B = data->pitch;  //D10
  OCR1C = data->roll;   //D6
  OCR1A = data->gaz;    //D9
}

// Function which handles the sonar request message
void HandleSonarRequest()
{
  // Map the outgoing buffer as a navdata struct
  sonarData* data = (sonarData*) &outgoingBuffer[1];
  
  // Set the baro & ultrasonic sensor value
  data->baro = control_timeout & 0xFFFF;
  data->ultrasonicSensor = GetSonarAltitude();
  
  //copy raw data
  memcpy(data->us_raw, lastUS_raw, 8);
  
  data->lightSensor = lastUltrasonicLight;
  
  // Send the message
  SendOutgoingMessage();
}

// Handles a request for angle gyro readings
void HandleGyroAngleRequest()
{
    gyroAngleData* data = (gyroAngleData*) &outgoingBuffer[1];
    data->roll = GetGyroRollRad();
    data->pitch = GetGyroPitchRad();

    SendOutgoingMessage();
}

void HandleIsAutnomousMessage()
{  
  isAutonomousData* data = (isAutonomousData*) &outgoingBuffer[1];
  
  // If the lenght is greather than 
  if(AutonomousChannel.value > CONTROL_DEFAULT )
  {
    data->isAutonomous = 1;
  }
  else
  {
    data->isAutonomous = 0;
  }

  SendOutgoingMessage();
}

// Function which handles the test message
void HandleTestMessage()
{
  testData* data = (testData*) &outgoingBuffer[1];
  
  data->ack = 42;
  
  SendOutgoingMessage();
}
