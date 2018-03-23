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

#include "Boat.h"
#include "Arduino.h"

//12 bits PWM resolution
#define PWM_MAX_DUTY 4096  

//define which pins are used for the control channels (for arduino DUE: the high-accuracy PWM driver only works on pins 6 - 9)
#define MAIN_CHANNEL 9   
#define BOW_CHANNEL 8
#define RUDDER_CHANNEL 7

/*
 * 4096 is max so 100% dutycycle
 * both main and bowscrew need a 1,5ms base PWM to stand still
 * the rudder needs a 1,4ms dutycycle to stand in the middle and sail forward
 * to calculate the amount of bytes needed to achieve those times we can just use a scale 0-4096 = 0-20ms
 */

//define PWM base values
#define MAIN_PWM_MIN 287
#define MAIN_PWM_MAX 327
#define MAIN_PWM_MIDDLE 309

#define BOW_PWM_MIN 266
#define BOW_PWM_MAX 348
#define BOW_PWM_MIDDLE 309

#define RUDDER_PWM_MIN 185
#define RUDDER_PWM_MAX 389
#define RUDDER_PWM_MIDDLE 287

Boat::Boat(const char* name) : Device(name, "With this actuator you can control the boat")
{  
  
  // add the variables so they can be changed
  _AddVariable("Duty_Main", &_DUTY_MAIN);
  _AddVariable("Duty_Bow", &_DUTY_BOW);
  _AddVariable("Duty_Rudder", &_DUTY_RUDDER);
  
  _AddVariable("Dir_Main", &_DIR_MAIN);
  _AddVariable("Dir_Bow", &_DIR_BOW);
  _AddVariable("Dir_Rudder", &_DIR_RUDDER);

  //setup PWM
  //50Hz @ 12 bits (4096)
  pmc_enable_periph_clk(PWM_INTERFACE_ID);
  PWMC_ConfigureClocks(50 * PWM_MAX_DUTY, 0, VARIANT_MCK);                

  //here we set the length of each PWM signal (the base value)
  InitDuty(MAIN_CHANNEL, MAIN_PWM_MIDDLE);
  InitDuty(BOW_CHANNEL, BOW_PWM_MIDDLE);
  InitDuty(RUDDER_CHANNEL, RUDDER_PWM_MIDDLE);
}




void Boat::Update()
{
  //for the rudder we dont need to wait before it stopped we can send it to left and right directly without breaking it
  SetDutyCycle_Rudder(_DUTY_RUDDER,_DIR_RUDDER);

  //printing all the received values
  Serial.print(" [duty-main = "); Serial.print(_DUTY_MAIN); Serial.print(" dir-main = "); Serial.print(_DIR_MAIN); Serial.print("]");
  Serial.print(" [duty-bow = "); Serial.print(_DUTY_BOW); Serial.print(" dir-bow = "); Serial.print(_DIR_BOW); Serial.print("]");
  Serial.print(" [duty-rudder = "); Serial.print(_DUTY_RUDDER); Serial.print(" dir-rudder = "); Serial.print(_DIR_RUDDER); Serial.println("]");
  /*
   * however for the motors we need to wait a moment:
   * if we switch direction fast on both motors they will break. Therefor those cases are created so we dont break em if we switch direction quickly.
   * if we switch direction we need to wait a while untill the motors stopped before we can send em steering/thrusting the other way.
   */
  switch(state){
    case 0:
      if(_DIR_MAIN != prev_DIR_MAIN){
        state = 1;
        prev_DIR_MAIN = _DIR_MAIN;
        lastUpdate = micros ();
      }
      if(_DIR_BOW != prev_DIR_BOW){
        state = 3;
        prev_DIR_BOW = _DIR_BOW;
        lastUpdate = micros ();
      }
      else {
        SetDutyCycle_Main(_DUTY_MAIN, _DIR_MAIN);
        SetDutyCycle_Bow(_DUTY_BOW, _DIR_BOW);
      }
      break;
    
    
    //cases for the main motor
    case 1:
      SetDutyCycle_Main(0, 0);        //Stop the motor
      if(lastUpdate < (micros() - 50000)){  //wait 50ms then switch to state 2
        state = 2;
        lastUpdate = micros ();
      }
     break;
    case 2:
      SetDutyCycle_Main(_DUTY_MAIN, _DIR_MAIN); //set direction and cycle to the new valeus
      if(lastUpdate < (micros() - 50000)){                        //wait again 50ms then switch to state 0
      state = 0;
      }
    break;
    
    
    //cases for the bow motor
    case 3:
      SetDutyCycle_Bow(0, 0);         //Stop the motor
      if(lastUpdate < (micros() - 50000)){  //wait 50ms then switch to state 4
        state = 4;
        lastUpdate = micros ();
      }
    break;
    case 4:
      SetDutyCycle_Bow(_DUTY_BOW, _DIR_BOW);  //set direction and cycle to the new valeus
      if(lastUpdate < (micros() - 50000)){                        //wait again 50ms then switch to state 0
      state = 0;
      }
    break;
  }
}







int Boat::InitDuty(int pin, int dutycycle)
{
  /*
   * In here we set the dutycycle for the pwm pin
   * The dutycycle is in bytes
   */
  PIO_Configure(g_APinDescription[pin].pPort,
    g_APinDescription[pin].ulPinType,
    g_APinDescription[pin].ulPin,
    g_APinDescription[pin].ulPinConfiguration);
  PWMC_ConfigureChannel(PWM_INTERFACE, g_APinDescription[pin].ulPWMChannel, PWM_CMR_CPRE_CLKA, 0, 0); 
  PWMC_SetPeriod(PWM_INTERFACE, g_APinDescription[pin].ulPWMChannel, PWM_MAX_DUTY);    
  PWMC_EnableChannel(PWM_INTERFACE, g_APinDescription[pin].ulPWMChannel);
  PWMC_SetDutyCycle(PWM_INTERFACE, g_APinDescription[pin].ulPWMChannel, dutycycle);
}






void Boat::SetDutyCycle_Main(int duty_Main, int dir_Main)                                 
{   
    int dir_Main_Factor = 0;
    duty_Main = map(duty_Main, 0, 100, 0, 20);
    /*
     * duty_Main should be 0-20 if the number is higher the rudder might go to fast and break
     * the dir is 0,1 or 2. 2 for forward, 0 for backward, 1 means that the motor will stand still
     */
    if (dir_Main == 0){
      dir_Main_Factor = -1;
    }
    else if (dir_Main == 2){
      dir_Main_Factor = 1;
    }
    else{                        //if the usb buffer sends stupid shit wich is not 0 or 2, it wil simply do nothing with it because the factor is 0.
      dir_Main_Factor = 0;
    }
    
    duty_Main = duty_Main * dir_Main_Factor;    
    if (duty_Main >= 20) duty_Main = 20;            //a limiter so that the dutycycle doesnt exceed the maximum number so it wont break the motor/servo  
    if (duty_Main <= -20) duty_Main = -20;                                    
    PWMC_SetDutyCycle(PWM_INTERFACE, g_APinDescription[MAIN_CHANNEL].ulPWMChannel, MAIN_PWM_MIDDLE + duty_Main); //base ~1,5ms = 309 bytes
}




void Boat::SetDutyCycle_Bow(int duty_Bow, int dir_Bow)                                   
{   
    int dir_Bow_Factor = 0;
    duty_Bow = map(duty_Bow, 0, 100, 0, 41);
    /*
     * duty_Bow_screw should be between 0-41
     * the dir is 0,1 or 2. I didnt use the bow thruster since the boat was unstable and the bow thruster wasnt always in the water
     */
    if (dir_Bow == 0){
      dir_Bow_Factor = -1;
    }
    else if (dir_Bow == 2){
      dir_Bow_Factor = 1;
    }
    else {                      //if the usb buffer sends stupid shit wich is not 0 or 2, it wil simply do nothing with it because the factor is 0.
      dir_Bow_Factor = 0;
    }
    duty_Bow = duty_Bow * dir_Bow_Factor;   
    if (duty_Bow >= 41) duty_Bow = 41;            //a limiter so that the dutycycle doesnt exceed the maximum number so it wont break the motor/servo   
    if (duty_Bow <= -41) duty_Bow = -41;                                 
    PWMC_SetDutyCycle(PWM_INTERFACE, g_APinDescription[BOW_CHANNEL].ulPWMChannel, BOW_PWM_MIDDLE + duty_Bow);  //base ~1,5ms = 309 bytes  
}




void Boat::SetDutyCycle_Rudder(int duty_Rudder, int dir_Rudder)                                             
{
    int dir_Rudder_Factor = 0;
    
    duty_Rudder = map(duty_Rudder, 0, 100, 0, 100);
    
    /*
     * duty_Rudder should be between 0-102
     * the dir is 0,1 or 2. 2 for right, 0 for left, 1 means the rudder wil sit in the middle
     */
    if (dir_Rudder == 0){
      dir_Rudder_Factor = -1;
    }
    else if (dir_Rudder == 2){
      dir_Rudder_Factor = 1;
    }
    else{                     //if the usb buffer sends stupid shit wich is not 0 or 2, it wil simply do nothing with it because the factor is 0.
      dir_Rudder_Factor = 0;
    }
      
    duty_Rudder = duty_Rudder * dir_Rudder_Factor; 
    
    //if (duty_Rudder >= 102) duty_Rudder = 102;                //a limiter so that the dutycycle doesnt exceed the maximum number so it wont break the motor/servo   
    //if (duty_Rudder <= -102) duty_Rudder = -102;                                                
    
    
    PWMC_SetDutyCycle(PWM_INTERFACE, g_APinDescription[RUDDER_CHANNEL].ulPWMChannel, RUDDER_PWM_MIDDLE + duty_Rudder);      //base ~1,4ms = 287 bytes           
}









































































//there no easter eggs here.
