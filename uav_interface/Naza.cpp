#include "Naza.h"
#include "Arduino.h"

//225, 900
//#define PWM_MIN 2250
#define PWM_MIN 9000
//389, 1556
//#define PWM_MAX 3890
#define PWM_MAX 15560
//307, 1228
//#define PWM_MIDDLE 3070
#define PWM_MIDDLE 12280

#define PITCH_CHANNEL 7
#define ROLL_CHANNEL 6
#define YAW_CHANNEL 9
#define GAZ_CHANNEL 8

Naza* Naza::_instance = nullptr;

Naza::Naza(const char* name) : Device(name, "With this actuator you can control Naza flight controllers")
{
  //setup PWM
  //12 bit, 500Hz
  pmc_enable_periph_clk( PWM_INTERFACE_ID );
  int clockFreq = 0x00003FFF * 500;
  PWMC_ConfigureClocks( clockFreq, clockFreq, VARIANT_MCK ); //both clock A and B set to 500Hz
  
  //configure the pins for PWM
  for(int i = 6; i <= 9; i++)
  {
    uint32_t chan = g_APinDescription[i].ulPWMChannel; 
    PIO_Configure( g_APinDescription[i].pPort, g_APinDescription[i].ulPinType, g_APinDescription[i].ulPin, g_APinDescription[i].ulPinConfiguration);
    PWMC_ConfigureChannel(PWM_INTERFACE, chan, PWM_CMR_CPRE_CLKA, 0, 0);
    PWMC_SetPeriod(PWM_INTERFACE, chan, 0x00003FFF);  //set for 12-bit pwm
    PWMC_SetDutyCycle(PWM_INTERFACE, chan, 0); // The 0 is the initial duty cycle
    PWMC_EnableChannel(PWM_INTERFACE, chan); 
  }
  
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
  _gaz = 0.0f;
    
  writeDefaultStickValues();
}

Naza::Naza(const char* name, GR12 *gr12) : Naza(name)
{
  _gr12 = gr12;
  _AddVariable("auto_pitch", &_auto_pitch);
  _AddVariable("auto_roll", &_auto_roll);
  _AddVariable("auto_yaw", &_auto_yaw);
  _AddVariable("auto_gaz", &_auto_gaz);
}

void Naza::writeDefaultStickValues()
{
  if(_auto_pitch == 1) {
    PWMC_SetDutyCycle(PWM_INTERFACE, g_APinDescription[PITCH_CHANNEL].ulPWMChannel, PWM_MIDDLE);
  }
  
  if(_auto_roll == 1) {
    PWMC_SetDutyCycle(PWM_INTERFACE, g_APinDescription[ROLL_CHANNEL].ulPWMChannel, PWM_MIDDLE);
  }
  
  if(_auto_yaw == 1) {
    PWMC_SetDutyCycle(PWM_INTERFACE, g_APinDescription[YAW_CHANNEL].ulPWMChannel, PWM_MIDDLE);
  }
  
  if(_auto_gaz == 1) {
    PWMC_SetDutyCycle(PWM_INTERFACE, g_APinDescription[GAZ_CHANNEL].ulPWMChannel, PWM_MIDDLE); 
  }
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

#define CLAMP(X) (X) = ((X) < -1.0f) ? -1.0f : ((X) > 1.0f) ? 1.0f : (X);
void Naza::ValuesChanged()
{
  if(_timeout > 0)
  {
    if(_auto_pitch == 1) {
      CLAMP(_pitch);
      uint16_t pulselengthPitch = map((_pitch * 10000.0f), -10000, 10000, PWM_MIN, PWM_MAX);
      PWMC_SetDutyCycle(PWM_INTERFACE, g_APinDescription[PITCH_CHANNEL].ulPWMChannel, pulselengthPitch);
    }

    if(_auto_roll == 1) {
      CLAMP(_roll);
      uint16_t pulselengthRoll = map((_roll * 10000.0f), -10000, 10000, PWM_MIN, PWM_MAX);
      PWMC_SetDutyCycle(PWM_INTERFACE, g_APinDescription[ROLL_CHANNEL].ulPWMChannel, pulselengthRoll);
    }

    if(_auto_yaw == 1) {
      CLAMP(_yaw);
      uint16_t pulselengthYaw = map((_yaw * 10000.0f), -10000, 10000, PWM_MIN, PWM_MAX);
      PWMC_SetDutyCycle(PWM_INTERFACE, g_APinDescription[YAW_CHANNEL].ulPWMChannel, pulselengthYaw);
    }

    if(_auto_gaz == 1) {
      CLAMP(_gaz);   
      uint16_t pulselengthGaz = map((_gaz * 10000.0f), -10000, 10000, PWM_MIN, PWM_MAX);
      PWMC_SetDutyCycle(PWM_INTERFACE, g_APinDescription[GAZ_CHANNEL].ulPWMChannel, pulselengthGaz);
    }
  }
}

void Naza::Update()
{
  if(_gr12 != nullptr) {
    if(_auto_pitch == 0) {
      uint16_t pulselengthPitch = map((_gr12->getPitch() * 10000.0f), -10000, 10000, PWM_MIN, PWM_MAX);
      PWMC_SetDutyCycle(PWM_INTERFACE, g_APinDescription[PITCH_CHANNEL].ulPWMChannel, pulselengthPitch);
    }

    if(_auto_roll == 0) {
      uint16_t pulselengthRoll = map((_gr12->getRoll() * 10000.0f), -10000, 10000, PWM_MIN, PWM_MAX);
      PWMC_SetDutyCycle(PWM_INTERFACE, g_APinDescription[ROLL_CHANNEL].ulPWMChannel, pulselengthRoll);
    }

    if(_auto_yaw == 0) {
      uint16_t pulselengthYaw = map((_gr12->getYaw() * 10000.0f), -10000, 10000, PWM_MIN, PWM_MAX);
      PWMC_SetDutyCycle(PWM_INTERFACE, g_APinDescription[YAW_CHANNEL].ulPWMChannel, pulselengthYaw);
    }

    if(_auto_gaz == 0) {
      uint16_t pulselengthGaz = map((_gr12->getGaz() * 10000.0f), -10000, 10000, PWM_MIN, PWM_MAX);
      PWMC_SetDutyCycle(PWM_INTERFACE, g_APinDescription[GAZ_CHANNEL].ulPWMChannel, pulselengthGaz);
    }
  }
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

Naza* Naza::Initialize(const char* name)
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


