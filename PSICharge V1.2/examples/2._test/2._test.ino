#include "PSI_CHARGE.h"

Pwm pilotPWM(true);
L1L2Sense test(2,3);
CTselfTest CTself(6,7);
GFCITest GFCI(8);
ledLights led(2,3,4);

void setup() {
  //put your setup code here, to run once:
  Serial.begin(9600);
  pilotPWM.setPWM(10);

  pinMode(LED_BUILTIN, OUTPUT);

 while(CTself.check()){
    CTself.errorLights();
  }

  
 while(GFCI.check()){
    GFCI.errorLights();
  }
  
  }
  
void loop() {
  test.check() ? Serial.println("pass") : Serial.println("fail");
  delay(500);
  led.setColor(60,75,128);
  
}
