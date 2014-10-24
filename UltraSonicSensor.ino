//settings for SRF08
#ifdef SONAR_TYPE_SRF08
  #define SONAR_START_ADDR 0x01
  #define SONAR_READ_BYTES 9
  #define SONAR_READ_VALUES 4
#endif

//settings for SRF02
#ifdef SONAR_TYPE_SRF02
  #define SONAR_START_ADDR 0x02
  #define SONAR_READ_BYTES 4
  #define SONAR_READ_VALUES 2
#endif

void setupUltrasonicSensor()
{
  //SRF02: initialize unused raw ultrasonic values to zero
  #ifdef SONAR_TYPE_SRF02
    lastUS_raw[2] = 0;
    lastUS_raw[3] = 0;
  #endif
  
  //SRF08: setup gain and range
  #ifdef SONAR_TYPE_SRF08
    Wire.beginTransmission(ULTRASONIC_I2CADDR); // transmit to SRF08
    Wire.write(byte(0x01));      // sets register pointer to gain register
    
    //write gain and range settings to sonar
    Wire.write(byte(ULTRASONIC_GAIN));
    Wire.write(byte(ULTRASONIC_RANGE));
    
    Wire.endTransmission();      // stop transmitting
  #endif
  
  startUltrasonicRanging();
}

bool updateUltrasonicSensor()
{
  //Read register 0 (version reg) of sonar. If the sonar is currently ranging this read is specified to return 0xFF. 
  Wire.beginTransmission(ULTRASONIC_I2CADDR);
  Wire.write(byte(0x00));                 //set register pointer to reg 0
  Wire.endTransmission();
  
  Wire.requestFrom(ULTRASONIC_I2CADDR, 1);
  unsigned char complete = Wire.read();   //read register
  
  //check if sonar is idle
  if(complete != 0xFF)
  {
    //read most recent data from sonar
    readUltrasonicSensorValue();
    //start new ranging
    startUltrasonicRanging();
    
    return true;
  }
  else return false;
}

// Function which gives the ultrasonic sensor the instruction to start a measurement 
void startUltrasonicRanging()
{
  // step 1: instruct sensor to read echoes
  Wire.beginTransmission(ULTRASONIC_I2CADDR); //transmit to sonar address
  Wire.write(byte(0x00));      // sets register pointer to the command register (0x00)  
  Wire.write(byte(0x51));      // command sensor to measure in "centimeters" (0x51) 
                               // use 0x50 for inches
                               // use 0x52 for ping microseconds
  Wire.endTransmission();      // stop transmitting
  
  //rangingStarted = msTimer;
 } 
 
 // Function which reads the value from the ultrasonic sensor.
 void readUltrasonicSensorValue()
 {
   // step 3: instruct sensor to return a particular echo reading
  Wire.beginTransmission(ULTRASONIC_I2CADDR); // transmit to sonar
  Wire.write(byte(SONAR_START_ADDR));      // sets register pointer to required register
  Wire.endTransmission();      // stop transmitting

  // step 4: request reading from sensor
  Wire.requestFrom(ULTRASONIC_I2CADDR, SONAR_READ_BYTES);    // request 2 bytes from slave device #112

  // step 5: receive reading from sensor
  if(SONAR_READ_BYTES <= Wire.available())    // if two bytes were received
  {
    #ifdef SONAR_TYPE_SRF08
      lastUltrasonicLight = Wire.read();
    #endif
    
    for(int i = 0; i < SONAR_READ_VALUES; i++)
    {
      lastUS_raw[i] = Wire.read();
      lastUS_raw[i] = lastUS_raw[i] << 8;
      lastUS_raw[i] |= Wire.read();
    }
    
    //Put result in circular buffer
    lastUltrasonicReadings[ultraSonicReadingsPointer] = lastUS_raw[0];
    ultraSonicReadingsPointer++;
    if(ultraSonicReadingsPointer>=ULTRASONIC_LOOPSIZE)ultraSonicReadingsPointer=0;
    
    #if ULTRASONIC_DEBUGOUTPUT
      Serial.println(GetSonarAltitude());
    #endif
  } 
}

int cmp_ultrasonic(const void *a, const void *b)
{
  const unsigned short *ia = (const unsigned short *) a;
  const unsigned short *ib = (const unsigned short *) b;
  return (*ia>*ib) - (*ia<*ib);
}

/*
 * Get the current filtered sonar altitude, which is the median value over the most recent readings. 
 */
unsigned short GetSonarAltitude()
{
  unsigned short UltrasonicReadingsSort[ULTRASONIC_LOOPSIZE];

  memcpy(UltrasonicReadingsSort, lastUltrasonicReadings, ULTRASONIC_LOOPSIZE * sizeof(unsigned short));    //Copy current results from circular buffer..
  qsort(UltrasonicReadingsSort, ULTRASONIC_LOOPSIZE, sizeof(unsigned short), cmp_ultrasonic);               //..and sort them

  #if ULTRASONIC_DEBUGOUTPUT
    for(byte i = 0 ; i < ULTRASONIC_LOOPSIZE ; i++)
    {
      Serial.print( UltrasonicReadingsSort[i] );
      Serial.print(" ");
    }
    Serial.println("");
  #endif

  #if ULTRASONIC_LOOPSIZE % 2 == 0
    //If number of results is even, take middle 2 and average them
    return (UltrasonicReadingsSort[ULTRASONIC_LOOPSIZE >> 1] + UltrasonicReadingsSort[(ULTRASONIC_LOOPSIZE >> 1) - 1]) >> 1;
  #else
    //If number of results is odd, take middle result
    return UltrasonicReadingsSort[ULTRASONIC_LOOPSIZE >> 1];
  #endif
}
