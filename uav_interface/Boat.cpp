#include "Boat.h"
#include "Arduino.h"

#define PWM_MAX_DUTY 4096  //12 bits PWM resolution

#define Pin_Main_Thruster 7   //Here the pins are defined for the motors and servos
#define Pin_Bow_Thruster 8
#define Pin_Rudder 9

#define Initialise_Duty_Main_Thruster 309  //the base pwm valeus of the pins in bytes
#define Initialise_Duty_Bow_Thruster 309   //After some testing the motors fully standstill at 309 bytes. instead of the calculated 307.
#define Initialise_Duty_Rudder 287

/*
 * 4096 is max so 100% dutycycle
 * both main and bowscrew need a 1,5ms base PWM to stand still
 * the rudder needs a 1,4ms dutycycle to stand in the middle and sail forward
 * to calculate the amount of bytes needed to achieve those times we can just use a scale 0-4096 = 0-20ms
 *               base  low   high  diff  pin
 * main screw =  307   287   327   20    9
 * bow screw  =  307   266   348   41    8
 * Ruddder    =  287   185   389   102   7
 */

//in here we initialise the PWM for the 2 motors and 1 servo
Boat::Boat(const char* name) :
		Device(name, "With this actuator you can control Boat controllers")
{
	/*
	 *  some variables wich are used to transfer the dutycycle and direction variables over usb. Those should be the same as the ones on the odroid!
	 */

	//the dutycycle variables
	_AddVariable("Motor_Duty_Main_Thruster", &_DUTY_MAIN_THRUSTER);
	_AddVariable("Motor_Duty_Bow_Thruster", &_DUTY_BOW_THRUSTER);
	_AddVariable("Servo_Duty_Rudder", &_DUTY_RUDDER);

	//the direction variables
	_AddVariable("Motor_Dir_Main_Thruster", &_DIR_MAIN_THRUSTER);
	_AddVariable("Motor_Dir_Bow_Thruster", &_DIR_BOW_THRUSTER);
	_AddVariable("Servo_Dir_Rudder", &_DIR_RUDDER);

	//here we set the frequency
	//in our case it its 50Hz @ 12 bits (4096)
	pmc_enable_periph_clk(PWM_INTERFACE_ID);
	PWMC_ConfigureClocks(50 * PWM_MAX_DUTY, 0, VARIANT_MCK);

	//here we set the length of each PWM signal (the base value)
	InitialiseDutyCycle(Pin_Main_Thruster, Initialise_Duty_Main_Thruster);
	InitialiseDutyCycle(Pin_Bow_Thruster, Initialise_Duty_Bow_Thruster);
	InitialiseDutyCycle(Pin_Rudder, Initialise_Duty_Rudder);
}

void Boat::Update()
{
	//for the rudder we dont need to wait before it stopped we can send it to left and right directly without breaking it
	SetDutyCycle_Rudder(_DUTY_RUDDER, _DIR_RUDDER);

	//printing all the received values
	//Serial.print(" [duty-main = "); Serial.print(_DUTY_MAIN_THRUSTER); Serial.print(" dir-main = "); Serial.print(_DIR_MAIN_THRUSTER); Serial.print("]");
	//Serial.print(" [duty-bow = "); Serial.print(_DUTY_BOW_THRUSTER); Serial.print(" dir-bow = "); Serial.print(_DIR_BOW_THRUSTER); Serial.print("]");
	//Serial.print(" [duty-rudder = "); Serial.print(_DUTY_RUDDER); Serial.print(" dir-rudder = "); Serial.print(_DIR_RUDDER); Serial.println("]");
	/*
	 * however for the motors we need to wait a moment:
	 * if we switch direction fast on both motors they will break. Therefor those cases are created so we dont break em if we switch direction quickly.
	 * if we switch direction we need to wait a while untill the motors stopped before we can send em steering/thrusting the other way.
	 */
	switch (state)
	{
	case 0:
		if (_DIR_MAIN_THRUSTER != prev_DIR_MAIN_THRUSTER)
		{
			state = 1;
			prev_DIR_MAIN_THRUSTER = _DIR_MAIN_THRUSTER;
			lastUpdate = micros();
		}
		if (_DIR_BOW_THRUSTER != prev_DIR_BOW_THRUSTER)
		{
			state = 3;
			prev_DIR_BOW_THRUSTER = _DIR_BOW_THRUSTER;
			lastUpdate = micros();
		}
		else
		{
			SetDutyCycle_Main_Thruster(_DUTY_MAIN_THRUSTER, _DIR_MAIN_THRUSTER);
			SetDutyCycle_Bow_Thruster(_DUTY_BOW_THRUSTER, _DIR_BOW_THRUSTER);
		}
		break;

		//cases for the main motor
	case 1:
		SetDutyCycle_Main_Thruster(0, 0);        //Stop the motor
		if (lastUpdate < (micros() - 50000))
		{  //wait 50ms then switch to state 2
			state = 2;
			lastUpdate = micros();
		}
		break;
	case 2:
		SetDutyCycle_Main_Thruster(_DUTY_MAIN_THRUSTER, _DIR_MAIN_THRUSTER);  //set direction and cycle to the new valeus
		if (lastUpdate < (micros() - 50000))
		{                        //wait again 50ms then switch to state 0
			state = 0;
		}
		break;

		//cases for the bow motor
	case 3:
		SetDutyCycle_Bow_Thruster(0, 0);         //Stop the motor
		if (lastUpdate < (micros() - 50000))
		{  //wait 50ms then switch to state 4
			state = 4;
			lastUpdate = micros();
		}
		break;
	case 4:
		SetDutyCycle_Bow_Thruster(_DUTY_BOW_THRUSTER, _DIR_BOW_THRUSTER);  //set direction and cycle to the new valeus
		if (lastUpdate < (micros() - 50000))
		{                        //wait again 50ms then switch to state 0
			state = 0;
		}
		break;
	}
}

int Boat::InitialiseDutyCycle(int pin, int dutycycle)
{
	/*
	 * In here we set the dutycycle for the pwm pin
	 * The dutycycle is in bytes
	 */
	PIO_Configure(g_APinDescription[pin].pPort, g_APinDescription[pin].ulPinType, g_APinDescription[pin].ulPin, g_APinDescription[pin].ulPinConfiguration);
	PWMC_ConfigureChannel(PWM_INTERFACE, g_APinDescription[pin].ulPWMChannel, PWM_CMR_CPRE_CLKA, 0, 0);
	PWMC_SetPeriod(PWM_INTERFACE, g_APinDescription[pin].ulPWMChannel, PWM_MAX_DUTY);
	PWMC_EnableChannel(PWM_INTERFACE, g_APinDescription[pin].ulPWMChannel);
	PWMC_SetDutyCycle(PWM_INTERFACE, g_APinDescription[pin].ulPWMChannel, dutycycle);

	return 0;
}

void Boat::SetDutyCycle_Main_Thruster(int duty_Main_Thruster, int dir_Main_Thruster)
{
	int dir_Main_Thruster_Factor = 0;
	duty_Main_Thruster = map(duty_Main_Thruster, 0, 100, 0, 20);
	/*
	 * duty_Main_Thruster should be 0-20 if the number is higher the rudder might go to fast and break
	 * the dir is 0,1 or 2. 2 for forward, 0 for backward, 1 means that the motor will stand still
	 */
	if (dir_Main_Thruster == 0)
	{
		dir_Main_Thruster_Factor = -1;
	}
	else if (dir_Main_Thruster == 2)
	{
		dir_Main_Thruster_Factor = 1;
	}
	else
	{                        //if the usb buffer sends stupid shit wich is not 0 or 2, it wil simply do nothing with it because the factor is 0.
		dir_Main_Thruster_Factor = 0;
	}

	duty_Main_Thruster = duty_Main_Thruster * dir_Main_Thruster_Factor;
	if (duty_Main_Thruster >= 20) duty_Main_Thruster = 20;  //a limiter so that the dutycycle doesnt exceed the maximum number so it wont break the motor/servo
	if (duty_Main_Thruster <= -20) duty_Main_Thruster = -20;
	PWMC_SetDutyCycle(PWM_INTERFACE, g_APinDescription[Pin_Main_Thruster].ulPWMChannel, Initialise_Duty_Main_Thruster + duty_Main_Thruster);  //base ~1,5ms = 309 bytes
}

void Boat::SetDutyCycle_Bow_Thruster(int duty_Bow_Thruster, int dir_Bow_Thruster)
{
	int dir_Bow_Thruster_Factor = 0;
	duty_Bow_Thruster = map(duty_Bow_Thruster, 0, 100, 0, 41);
	/*
	 * duty_Bow_screw should be between 0-41
	 * the dir is 0,1 or 2. I didnt use the bow thruster since the boat was unstable and the bow thruster wasnt always in the water
	 */
	if (dir_Bow_Thruster == 0)
	{
		dir_Bow_Thruster_Factor = -1;
	}
	else if (dir_Bow_Thruster == 2)
	{
		dir_Bow_Thruster_Factor = 1;
	}
	else
	{                      //if the usb buffer sends stupid shit wich is not 0 or 2, it wil simply do nothing with it because the factor is 0.
		dir_Bow_Thruster_Factor = 0;
	}
	duty_Bow_Thruster = duty_Bow_Thruster * dir_Bow_Thruster_Factor;
	if (duty_Bow_Thruster >= 41) duty_Bow_Thruster = 41;  //a limiter so that the dutycycle doesnt exceed the maximum number so it wont break the motor/servo
	if (duty_Bow_Thruster <= -41) duty_Bow_Thruster = -41;
	PWMC_SetDutyCycle(PWM_INTERFACE, g_APinDescription[Pin_Bow_Thruster].ulPWMChannel, Initialise_Duty_Bow_Thruster + duty_Bow_Thruster);  //base ~1,5ms = 309 bytes
}

void Boat::SetDutyCycle_Rudder(int duty_Rudder, int dir_Rudder)
{
	int dir_Rudder_Factor = 0;

	duty_Rudder = map(duty_Rudder, 0, 100, 0, 100);

	/*
	 * duty_Rudder should be between 0-102
	 * the dir is 0,1 or 2. 2 for right, 0 for left, 1 means the rudder wil sit in the middle
	 */
	if (dir_Rudder == 0)
	{
		dir_Rudder_Factor = -1;
	}
	else if (dir_Rudder == 2)
	{
		dir_Rudder_Factor = 1;
	}
	else
	{                     //if the usb buffer sends stupid shit wich is not 0 or 2, it wil simply do nothing with it because the factor is 0.
		dir_Rudder_Factor = 0;
	}

	duty_Rudder = duty_Rudder * dir_Rudder_Factor;

	//if (duty_Rudder >= 102) duty_Rudder = 102;                //a limiter so that the dutycycle doesnt exceed the maximum number so it wont break the motor/servo
	//if (duty_Rudder <= -102) duty_Rudder = -102;

	Serial.println(Initialise_Duty_Rudder + duty_Rudder);

	PWMC_SetDutyCycle(PWM_INTERFACE, g_APinDescription[Pin_Rudder].ulPWMChannel, Initialise_Duty_Rudder + duty_Rudder);     //base ~1,4ms = 287 bytes
}

//there no easter eggs here.
