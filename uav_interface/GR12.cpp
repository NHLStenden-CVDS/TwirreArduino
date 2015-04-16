#DEFINE PWM_FREQUENCY 50

void setup() {
  volatile uint64_t time0;  
  volatile uint64_t lastDutyCycle;
  
  attachInterrupt(0, handleRisingPort0, RISING);
  attachInterrupt(0, handleFallingPort0, FALLING);
}

void loop() {
 
}

void handleRisingPort0()
{
  time0 micros();
}

void handleFallingPort0()
{
  lastDutyCycle = micros() - time0;
}

double dutyCycleToInputValue(uint64_t dutyCycle)
{
}