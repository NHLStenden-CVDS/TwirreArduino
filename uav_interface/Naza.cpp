#include "Naza.h"
#include "Arduino.h"

//#include "Adafruit_PWMServoDriver.h"
//225, 900
#define PWM_MIN 2250
//389, 1556
#define PWM_MAX 3890
//307, 1228
#define PWM_MIDDLE 3070

#define PITCH_CHANNEL 6
#define ROLL_CHANNEL 7
#define YAW_CHANNEL 8
#define GAZ_CHANNEL 9

Naza* Naza::_instance = nullptr;

Naza::Naza(char* name) : Device(name, "With this actuator you can control Naza flight controllers")
{
  // 12 bits PWM resolution
  analogWriteResolution(12);
  
  _timeout = 0;
  
  // TC1 channel 0, the IRQ for that channel and the desired frequency
  startTimer(TC1, 0, TC3_IRQn, 1000); 
   
  // we add the variables so they can be changed
  _AddVariable("pitch", &_pitch);
  _AddVariable("roll", &_roll);
  _AddVariable("yaw", &_yaw);
  _AddVariable("gaz", &_gaz);
  _AddVariable("timeout", &_timeout);
  
  _pitch = 0.0f;
  _roll = 0.0f;
  _yaw = 0.0f;
  _gaz = -1.0f;
    
  writeDefaultStickValues();
  analogWrite(GAZ_CHANNEL, PWM_MIN);
}

void Naza::writeDefaultStickValues()
{
  analogWrite(PITCH_CHANNEL, PWM_MIDDLE);
  analogWrite(ROLL_CHANNEL, PWM_MIDDLE);
  analogWrite(YAW_CHANNEL, PWM_MIDDLE);
  analogWrite(GAZ_CHANNEL, PWM_MIDDLE);
}

//timer handler TC1 ch 0
void TC3_Handler()
{
  // Clear interrupt status
  TC_GetStatus(TC1, 0);
  
  uint32_t* timeout = Naza::Instance()->getTimeout();
  if(*timeout == 0) return;
  
  if(--*timeout == 0)
  {
    Naza::Instance()->writeDefaultStickValues();
  }
}

void Naza::ValuesChanged()
{
  if(_timeout > 0)
  {  
    uint16_t pulselengthPitch = map((_pitch * 10000.0f), -10000, 10000, PWM_MIN, PWM_MAX);
    uint16_t pulselengthRoll = map((_roll * 10000.0f), -10000, 10000, PWM_MIN, PWM_MAX);
    uint16_t pulselengthYaw = map((_yaw * 10000.0f), -10000, 10000, PWM_MIN, PWM_MAX);
    uint16_t pulselengthGaz = map((_gaz * 10000.0f), -10000, 10000, PWM_MIN, PWM_MAX);
  
    analogWrite(PITCH_CHANNEL, pulselengthPitch);
    analogWrite(ROLL_CHANNEL, pulselengthRoll);
    analogWrite(YAW_CHANNEL, pulselengthYaw);
    analogWrite(GAZ_CHANNEL, pulselengthGaz);
  }
}

// TODO: Add timeout to interupts
int out = PWM_MIN;
int dir = 1;
void Naza::Update()
{
}

uint32_t * Naza::getTimeout()
{
  return &_timeout;
}

void Naza::startTimer(Tc *tc, uint32_t channel, IRQn_Type irq, uint32_t frequency) {
  pmc_set_writeprotect(false);
  pmc_enable_periph_clk((uint32_t)irq);
  TC_Configure(tc, channel, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK4);
  uint32_t rc = VARIANT_MCK/128/frequency; //128 because we selected TIMER_CLOCK4 above
  TC_SetRA(tc, channel, rc/2); //50% high, 50% low
  TC_SetRC(tc, channel, rc);
  TC_Start(tc, channel);
  tc->TC_CHANNEL[channel].TC_IER=TC_IER_CPCS;
  tc->TC_CHANNEL[channel].TC_IDR=~TC_IER_CPCS;
  NVIC_EnableIRQ(irq);
}

Naza* Naza::Initialize(char* name)
{
  if(_instance == nullptr)
  {
    _instance = new Naza(name);
  }
  return _instance;    
}

Naza* Naza::Instance()
{
    return _instance;
}


