
// The packet size of the incoming messages
#define PACKET_SIZE 16

//neutral control positions (1100 to 1900)
#define CONTROL_NEUTRAL 1500
#define CONTROL_MIN 1100

//default control position, logged when signal is lost
#define CONTROL_DEFAULT 1500

//PWM input timeout in uSecond
#define RC_TIMEOUT 100000 

//control modes
#define CONTROLMODE_GPS 233
#define CONTROLMODE_ATTI 190
#define CONTROLMODE_MAN 149

/*
 * select correct sonar here!
 */
//Use SRF08 sonar
#define SONAR_TYPE_SRF08
//use SRF02 sonar
// #define SONAR_TYPE_SRF02

//settings for SRF08
//see also: http://www.robot-electronics.co.uk/htm/srf08tech.shtml
#ifdef SONAR_TYPE_SRF08
  //I2C address of sonar
  #define ULTRASONIC_I2CADDR 112
  //Sonar RX gain factor. Reduce if the sonar picks up 'false' echoes. Increase if the sonar fails to receive the correct echo. (0x1F is max and default, but caused severe ghost echo problems)
  #define ULTRASONIC_GAIN 0x00
  //Sonar ranging limit - lower values will cause lower maximum range, but increases refresh rate. Lower ranges require a lower gain value. Default = 0xFF (11 meters). As the SRF08 cannot reliably range beyond 6 meters, this setting is recommended to be 0x8C (6 meters).
  #define ULTRASONIC_RANGE 0x8C
#endif

//settings for SRF02
//see also: http://www.robot-electronics.co.uk/htm/srf02techI2C.htm
#ifdef SONAR_TYPE_SRF02
  //I2C address of sonar
  #define ULTRASONIC_I2CADDR 120
  //Unused in SRF02
  #define ULTRASONIC_GAIN 0x00
  //Unused in SRF02
  #define ULTRASONIC_RANGE 0x00
#endif

//should a beeper/led be used to signal InterfaceX startup
//#define USE_BEEPER
//pin of beeper/led
#define BEEPERPIN 13

//Display median filter debuginfo
#define ULTRASONIC_DEBUGOUTPUT false
//Circular buffer size for median filter
#define ULTRASONIC_LOOPSIZE 7

#include <Wire.h>

////////////////////////////////////////////////////////////
// Structs for incomming PWM signals                      //
////////////////////////////////////////////////////////////
struct RCChannel {
   byte pin;
   unsigned long lastUpdate;
   boolean state;
   unsigned int value;  
};



RCChannel AutonomousChannel = { 7, micros(), 0, CONTROL_DEFAULT };
//RCChannel RCchanB = { 8, current, 0, CONTROL_DEFAULT };
//RCChannel RCchanC = { 15, current, 0, CONTROL_DEFAULT };
//RCChannel RCchanD = { 16, current, 0, CONTROL_DEFAULT };




////////////////////////////////////////////////////////////
// Structs for mapping the data of the incoming messages //
////////////////////////////////////////////////////////////

#pragma pack(push, 1) //set 1-byte element alignment (effectively disables automatic struct padding)
// Struct for the control message data
struct controlData
{
  uint32_t timeout;
  unsigned short yaw;
  unsigned short pitch;
  unsigned short roll;
  unsigned short gaz;
  unsigned char mode;
};
#pragma pack(pop) //restore previous padding settings

///////////////////////////////////////////////////////////
// Structs for mapping the data of the outgoing messages //
///////////////////////////////////////////////////////////

#pragma pack(push, 1) //set 1-byte element alignment (effectively disables automatic struct padding)
struct sonarData
{
  unsigned short baro;               //barometer value (unimplemented)
  unsigned short ultrasonicSensor;   //median value of last n sonar readings (n = ULTRASONIC_LOOPSIZE)
  unsigned short us_raw[4];          //raw sonar values (index 1 through 3 are invalid in case SRF02 is used)
  unsigned char lightSensor;         //value for the sonar's light sensor (found on SRF08)
};

struct rcData
{
  unsigned short RCChanA;
  unsigned short RCChanB;
  unsigned short RCChanC;
  unsigned short RCChanD; 
};

// Struct for the test message data
struct testData
{
  short ack;
};

// Struct for the isAutonomous message data
struct isAutonomousData
{
  unsigned long isAutonomous;
};

struct gyroAngleData
{
    float roll;
    float pitch;
};

struct gyroData
{
    float roll;
    float pitch;
    float droll;
    float dpitch;
    float dyaw;
    float temperature;
};

#pragma pack(pop) //restore previous padding settings

// byte array where the incoming messages are stored
byte incomingBuffer[PACKET_SIZE];

// byte array where the outgoing messages are created
byte outgoingBuffer[PACKET_SIZE];

//NAZA control mode
static int16_t ControlMode;

//raw values of last sonar reading (SRF02: <distance, ranging minimum, N/A, N/A>; SRF08: <echo1 distance, echo2 distance, echo3 distance, echo4 distance>)
unsigned short lastUS_raw[4];
unsigned char lastUltrasonicLight = 0x80;
byte ultraSonicReadingsPointer = 0;
unsigned short lastUltrasonicReadings[ULTRASONIC_LOOPSIZE];


// timer which contains the current running time in ms.
volatile unsigned long msTimer = 0;

// timeout for control values. When this timeout expires all controls will be set to neutral position
volatile uint32_t control_timeout;

// timeout timer interrupt
ISR( TIMER0_COMPA_vect )
{
  msTimer++;
  
  //no timeout active, return immediately
  if(control_timeout == 0) return;
  
  if(--control_timeout == 0)
  {
    //timeout expired: set control values to neutral position
    OCR3A = CONTROL_NEUTRAL;
    OCR1B = CONTROL_NEUTRAL;
    OCR1C = CONTROL_NEUTRAL;
    OCR1A = CONTROL_NEUTRAL;
  }
}

//program initialization, setup timers
void setup()
{
  // join i2c bus (address optional for master)
  Wire.begin();               
  
  // Setting up the serial connection
  Serial.begin(115200);
  
  // Set pin 7 for input. Pin 7 measures if the UAV is flying autonomously or not.
  pinMode(7, INPUT);
  
  //For 1ms timer interrupt
  DDRB |= (1 << 7);
  TCCR0A = _BV( WGM01 );                //timer0 mode = CTC(WGM01) 
  TCCR0B = _BV( CS01 ) | _BV( CS00 );   //Prescaler = 64(CS01 | CS00 )
  OCR0A = 248;                          //comparematch = 248 (1ms)
  sei();                                //enable global interrupt
  TIMSK0 |= _BV( OCIE0A );              //enable comparematch interrupt(OCIE0A)

  // For D9, D10 and D11
  DDRB = 0xE0;
  TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(COM1C1);
  TCCR1B = _BV(WGM13) | _BV(CS11);
  ICR1 = 20000; //TOP (50 Hz) prescaler 8x
  
  // For D5
  DDRC = 0xC0;
  TCCR3A = _BV(COM3A1);
  TCCR3B = _BV(WGM13) | _BV(CS11);
  ICR3 = 20000; //TOP (50 Hz) prescaler 8x
  
  // For D6 and D13
  DDRD = 0x80;
  TCCR4A = _BV(COM4A1) | _BV(PWM4A);   //Connect pin 13 to timer 4 channel A (not working)
  TCCR4B = _BV(CS43);  	               //Prescaler 128
  TCCR4C = _BV(COM4D1) | _BV(PWM4D);   //connect pin 6 to timer 4 channel D
  TCCR4D = _BV(WGM40);                 //Phase and Frequency Correct PWM
  TCCR4E = _BV(ENHC4);                 //Enhanced PWM Mode
  
  static int16_t top = 961;            //TOP (65 Hz)
  TC4H = top>>8;                       //TOP highbyte 
  OCR4C = (top&0xFF);                  //TOP lowbyte
    
  OCR1A = CONTROL_NEUTRAL;      //gaz
  OCR1B = CONTROL_NEUTRAL;   //pitch
  OCR1C = CONTROL_NEUTRAL;   //roll
  OCR3A = CONTROL_NEUTRAL;   //yaw
   
  ControlMode = CONTROLMODE_ATTI; //Naza control mode
  TC4H = ControlMode>>8; //ControlMode highbyte
  OCR4D = (ControlMode&0xFF); //ControlMode lowbyte

  setupUltrasonicSensor();
  setupGyros();

#ifdef USE_BEEPER
  pinMode(BEEPERPIN,OUTPUT);
  beep();
#endif
}




//mainloop
void loop ()
{  
  // Read and handle incoming message
  ReadMessage();
  // Read ultrasonic sensor
  updateUltrasonicSensor();
  // Read gyro's
  ReadGyrosAverage();
  // Read the Autonomous Switch
  ReadStickInputs();
}


void beep()
{
  unsigned long startTime, prevTime, curTime;
  
  prevTime  = msTimer * 1000 + TCNT0 * 4 ;
  startTime = msTimer * 1000 + TCNT0 * 4 ;
  curTime   = msTimer * 1000 + TCNT0 * 4 ;
  
  while(curTime < startTime + 1000000){
    curTime = msTimer * 1000 + TCNT0 * 4 ;
    if(curTime > prevTime + 100000){
      digitalWrite(BEEPERPIN, !digitalRead(BEEPERPIN) );
      prevTime = curTime;
    }
  }
  digitalWrite(BEEPERPIN, LOW);
}


void ReadStickInputs()
{
  readRC(&AutonomousChannel);
}

void readRC(struct RCChannel *chan){
  if( digitalRead(chan->pin)!=chan->state ){
    
    unsigned long micro = msTimer * 1000 + TCNT0 * 4 ;

    //State change
    chan->state^=1;  //Invert state
    
    unsigned long diff = micro - chan->lastUpdate;
    chan->lastUpdate = micro;
    
    
    if(!chan->state){
      chan->value = diff;
    }
    
  }
}

void HandleStickInput()
{
  rcData * data = (rcData *) &outgoingBuffer[1];
  /*
  data->RCChanA = RCchanA.value;
  data->RCChanB = RCchanB.value;
  data->RCChanC = RCchanC.value;
  data->RCChanD = RCchanD.value;
*/
  SendOutgoingMessage();
}

