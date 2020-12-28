#include <PSI_CHARGE.h>

Pwm pilotPWM(true);

void setup()
{
  Serial.begin(9600);
  pilotPWM.setPWM(10);
}




void loop()
{
   
}
