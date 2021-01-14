#include <PSI_CHARGE.h>

// Declar pins for L1L2 Sense
ledLights led(2,3,4);


void setup() {

  Serial.begin(9600);
 
  }
  
void loop() {
  //set color by setting the PWM on selected pins. 
  led.setColor(60,75,128);
}
