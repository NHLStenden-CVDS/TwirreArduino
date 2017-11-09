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

#include "Naza.h"
#include "Arduino.h"

//1100µs @ 8.4MHz basefreq
#define PWM_MIN 9240

//1900µs @ 8.4MHz basefreq
#define PWM_MAX 15960

//1500µs @ 8.4MHz basefreq
#define PWM_MIDDLE 12600

//define which pins are used for the control channels (for arduino DUE: the high-accuracy PWM driver only works on pins 6 - 9)
#define ROLL_CHANNEL 9
#define PITCH_CHANNEL 8
#define GAZ_CHANNEL 7
#define YAW_CHANNEL 6

#define CLAMP(X) (X) = ((X) < -1.0f) ? -1.0f : ((X) > 1.0f) ? 1.0f : (X);
//define how big the naza deadzone is (with full control range being (-1,1) ). This will be corrected in software, eg. when a control output of 0.05 is requested, an actual output of approx. 0.15 is given to circumvent the NAZA controller's deadzone.
//NAZA controller deadzone was determined to be in (-0.1,0.1) range.
#define NAZA_DEADZONE 0.1

Naza* Naza::_instance = nullptr;

Naza::Naza(const char* name) : Device(name, "With this actuator you can control Naza flight controllers")
{
  //setup PWM
  //14 bit, 500Hz
  pmc_enable_periph_clk( PWM_INTERFACE_ID );
  int clockFreq = 0x00003FFF * 500;
  //this will actually set the base frequency to (CPU_FREQ / round(CPU_FREQ / clockFreq))!
  //meaning the actual PWM frequency will be 512.7Hz
  PWMC_ConfigureClocks( clockFreq, clockFreq, VARIANT_MCK ); //both clock A and B set to 500Hz
  
  //configure the pins for PWM
  for(int i = 6; i <= 9; i++)
  {
    uint32_t chan = g_APinDescription[i].ulPWMChannel; 
    PIO_Configure( g_APinDescription[i].pPort, g_APinDescription[i].ulPinType, g_APinDescription[i].ulPin, g_APinDescription[i].ulPinConfiguration);
    PWMC_ConfigureChannel(PWM_INTERFACE, chan, PWM_CMR_CPRE_CLKA, 0, 0);
    PWMC_SetPeriod(PWM_INTERFACE, chan, 0x00003FFF);  //set for 14-bit pwm
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
  _AddVariable("pitchDeadzone", &_pitch_deadzone);
  _AddVariable("rollDeadzone", &_roll_deadzone);
  _AddVariable("yawDeadzone", &_yaw_deadzone);
  _AddVariable("gazDeadzone", &_gaz_deadzone);
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

void Naza::ValuesChanged()
{
  //limit deadzone
  if(_pitch_deadzone > (2 * NAZA_DEADZONE) ) _pitch_deadzone = (2 * NAZA_DEADZONE);
  if(_roll_deadzone > (2 * NAZA_DEADZONE) ) _roll_deadzone = (2 * NAZA_DEADZONE);
  if(_yaw_deadzone > (2 * NAZA_DEADZONE) ) _yaw_deadzone = (2 * NAZA_DEADZONE);
  if(_gaz_deadzone > (2 * NAZA_DEADZONE) ) _gaz_deadzone = (2 * NAZA_DEADZONE);
  
  if(_timeout > 0)
  {
    if(_auto_pitch == 1) {
      CLAMP(_pitch);
      
      //fix naza deadzone
      float deadzone = NAZA_DEADZONE - _pitch_deadzone;
      float deadzone_inv = 1 - deadzone;
      float pitch_a = _pitch * deadzone_inv;
      if(pitch_a < 0) pitch_a -= deadzone;
      if(pitch_a > 0) pitch_a += deadzone;
      
      uint16_t pulselengthPitch = map((pitch_a * 10000.0f), -10000, 10000, PWM_MIN, PWM_MAX);
      PWMC_SetDutyCycle(PWM_INTERFACE, g_APinDescription[PITCH_CHANNEL].ulPWMChannel, pulselengthPitch);
    }

    if(_auto_roll == 1) {
      CLAMP(_roll);
      //fix naza deadzone
      float deadzone = NAZA_DEADZONE - _roll_deadzone;
      float deadzone_inv = 1 - deadzone;
      float roll_a = _roll * deadzone_inv;
      if(roll_a < 0) roll_a -= deadzone;
      if(roll_a > 0) roll_a += deadzone;
      uint16_t pulselengthRoll = map((roll_a * 10000.0f), -10000, 10000, PWM_MIN, PWM_MAX);
      PWMC_SetDutyCycle(PWM_INTERFACE, g_APinDescription[ROLL_CHANNEL].ulPWMChannel, pulselengthRoll);
    }

    if(_auto_yaw == 1) {
      CLAMP(_yaw);
      //fix naza deadzone
      float deadzone = NAZA_DEADZONE - _yaw_deadzone;
      float deadzone_inv = 1 - deadzone;
      float yaw_a = _yaw * deadzone_inv;
      if(yaw_a < 0) yaw_a -= deadzone;
      if(yaw_a > 0) yaw_a += deadzone;
      uint16_t pulselengthYaw = map((yaw_a * 10000.0f), -10000, 10000, PWM_MIN, PWM_MAX);
      PWMC_SetDutyCycle(PWM_INTERFACE, g_APinDescription[YAW_CHANNEL].ulPWMChannel, pulselengthYaw);
    }

    if(_auto_gaz == 1) {
      CLAMP(_gaz);
      //fix naza deadzone
      float deadzone = NAZA_DEADZONE - _gaz_deadzone;
      float deadzone_inv = 1 - deadzone;
      float gaz_a = _gaz * deadzone_inv;
      if(gaz_a < 0) gaz_a -= deadzone;
      if(gaz_a > 0) gaz_a += deadzone;   
      uint16_t pulselengthGaz = map((gaz_a * 10000.0f), -10000, 10000, PWM_MIN, PWM_MAX);
      PWMC_SetDutyCycle(PWM_INTERFACE, g_APinDescription[GAZ_CHANNEL].ulPWMChannel, pulselengthGaz);
    }
  }
  else
  {
    writeDefaultStickValues();
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

Naza* Naza::Initialize(const char* name, GR12 *gr12)
{
  if(_instance == nullptr)
  {
    _instance = new Naza(name, gr12);
  }
  return _instance;    
}

Naza* Naza::Instance()
{
    return _instance;
}


