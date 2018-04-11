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
#define MAIN_PWM_DEVIATION 20

#define BOW_PWM_MIN 266
#define BOW_PWM_MAX 348
#define BOW_PWM_MIDDLE 309
#define BOW_PWM_DEVIATION 41

#define RUDDER_PWM_MIN 185
#define RUDDER_PWM_MAX 389
#define RUDDER_PWM_MIDDLE 287
#define RUDDER_PWM_DEVIATION 102

#define FORWARD 1
#define STILL 0
#define BACKWARD -1

Boat::Boat(const char* name) : Device(name, "With this actuator you can control the boat")
{  

	// add the variables so they can be changed
	_AddVariable("Main_Dir", &_MAIN_DIR);
	_AddVariable("Main_Speed", &_MAIN_SPEED);
	_AddVariable("Bow_Dir", &_BOW_DIR);
	_AddVariable("Rudder_Angle", &_RUDDER_ANGLE);

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
	//printing all the received values
	//Serial.print(" [duty-main = "); Serial.print(_DUTY_MAIN); Serial.print(" dir-main = "); Serial.print(_DIR_MAIN); Serial.print("]");
	//Serial.print(" [duty-bow = "); Serial.print(_DUTY_BOW); Serial.print(" dir-bow = "); Serial.print(_DIR_BOW); Serial.print("]");
	//Serial.print(" [duty-rudder = "); Serial.print(_DUTY_RUDDER); Serial.print(" dir-rudder = "); Serial.print(_DIR_RUDDER); Serial.println("]");
	
	//Sending the values to decide dutycycles
	Set_Main(_MAIN_DIR, _MAIN_SPEED);
	Set_Bow(_BOW_DIR);
	Set_Rudder(_RUDDER_ANGLE);
}



/*
 * In here we set the dutycycle for the pwm pin
 * The dutycycle is in bytes
 */
int Boat::InitDuty(int pin, int dutycycle)
{
	PIO_Configure(g_APinDescription[pin].pPort,
		g_APinDescription[pin].ulPinType,
		g_APinDescription[pin].ulPin,
		g_APinDescription[pin].ulPinConfiguration);
	PWMC_ConfigureChannel(PWM_INTERFACE, g_APinDescription[pin].ulPWMChannel, PWM_CMR_CPRE_CLKA, 0, 0); 
	PWMC_SetPeriod(PWM_INTERFACE, g_APinDescription[pin].ulPWMChannel, PWM_MAX_DUTY);    
	PWMC_EnableChannel(PWM_INTERFACE, g_APinDescription[pin].ulPWMChannel);
	PWMC_SetDutyCycle(PWM_INTERFACE, g_APinDescription[pin].ulPWMChannel, dutycycle);
}




/*
 * however for the motors we need to wait a moment:
 * if we switch direction fast on both motors they will break. Therefor those cases are created so we dont break em if we switch direction quickly.
 * if we switch direction we need to wait a while untill the motors stopped before we can send em steering/thrusting the other way.
 */
void Boat::Set_Main(signed int dir, uint8_t speed)
{
	int max = MAIN_PWM_DEVIATION; //MAIN_PWM_MIDDLE - MAIN_PWM_MAX;
	duty_cycle = (dir*((max/100)*speed)) + MAIN_PWM_MIDDLE;
	
	switch(state)	{
		case 0:
			if(dir != old_dir)	{
				state = 1;
				old_dir = dir;
				last_update = micros()
			} else
				SetDutyCycle_Main(duty_cycle);
		break;
		case 1:
			//Stop de motor
			SetDutyCycle_Main(MAIN_PWM_MIDDLE);
			if(last_update < (micros() - 50000))	{	// wait 50ms
				state = 2;
				last_update = micros();
			}
		break;
		case 2:
			//Set the direction that dir indicates
			SetDutyCycle_Main(duty_cycle);
			if(last_update < (micros() - 50000))	{	// wait 50ms
				state = 0;
			}
		break;
	}
}




void Boat::SetDutyCycle_Main(int duty_Main)                                 
{                              
	PWMC_SetDutyCycle(PWM_INTERFACE, g_APinDescription[MAIN_CHANNEL].ulPWMChannel, duty_Main); //base ~1,5ms = 309 bytes
}




/*
 * however for the motors we need to wait a moment:
 * if we switch direction fast on both motors they will break. Therefor those cases are created so we dont break em if we switch direction quickly.
 * if we switch direction we need to wait a while untill the motors stopped before we can send em steering/thrusting the other way.
 */
void Boat::Set_Bow(signed int dir)
{
	int max = BOW_PWM_DEVIATION; //BOW_PWM_MAX - BOW_PWM_MIDDLE;
	duty_cycle = (max * dir) + BOW_PWM_MIDDLE;
	
	switch(state)	{
		case 0:
			if(dir != old_dir)	{
				state = 1;
				old_dir = dir;
				last_update = micros()
			} else
				SetDutyCycle_Main(duty_cycle);
		break;
		case 1:
			//Stop de motor
			SetDutyCycle_Bow(BOW_PWM_MIDDLE)
			if(last_update < (micros() - 50000))	{	// wait 50ms
				state = 2;
				last_update = micros();
			}
		break;
		case 2:
			//Set the direction that dir indicates
			SetDutyCycle_Bow(duty_cycle);
			if(last_update < (micros() - 50000))	{	// wait 50ms
				state = 0;
			}
		break;
	}
}




void Boat::SetDutyCycle_Bow(int duty_Bow)                                   
{                                  
	PWMC_SetDutyCycle(PWM_INTERFACE, g_APinDescription[BOW_CHANNEL].ulPWMChannel, duty_Bow);  //base ~1,5ms = 309 bytes  
}



void Boat::Set_Rudder(signed int angle_percentage)
{
	int max = RUDDER_PWM_DEVIATION;
	duty_cycle = ((max/100) * angle_percentage) + RUDDER_PWM_MIDDLE;
	SetDutyCycle_Rudder(duty_cycle);
}




void Boat::SetDutyCycle_Rudder(int duty_Rudder)                                             
{
	PWMC_SetDutyCycle(PWM_INTERFACE, g_APinDescription[RUDDER_CHANNEL].ulPWMChannel, duty_Rudder);      //base ~1,4ms = 287 bytes           
}

