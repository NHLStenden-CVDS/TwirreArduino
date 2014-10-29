#include "Sonar.h"

Sonar sonar;

void setup()
{
    Serial.begin(9600);
    sonar.Init();
}

void loop()
{
    sonar.Update();
    Serial.print(sonar._value);
    Serial.print("\n");
} 
