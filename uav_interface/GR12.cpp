#define PWM_FREQUENCY 50

volatile uint64_t time0;  
volatile uint64_t lastDutyCycle;

void setup() {
  attachInterrupt(0, handleRisingPort0, RISING);
  attachInterrupt(0, handleFallingPort0, FALLING);
}

void loop() {
 
}

void handleRisingPort0()
{
  time0 = micros();
}

void handleFallingPort0()
{
  lastDutyCycle = micros() - time0;
}

double dutyCycleToInputValue(uint64_t dutyCycle, uint16_t pwmFrequency, uint16_t minDutyCycle, uint16_t maxDutyCycle)
{
  // TODO: between -1 and 1.
  return  (dutyCycle - minDutyCycle) / (maxDutyCycle - minDutyCycle);
}