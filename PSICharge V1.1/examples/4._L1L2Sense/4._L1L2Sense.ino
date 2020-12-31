#include <PSI_CHARGE.h>

// Declar pins for L1L2 Sense
L1L2Sense Linesensing(2,3);


void setup() {

  Serial.begin(9600);
 
  }
  
void loop() {
  //check voltage on lines
  Linesensing.check() ? Serial.println("pass") : Serial.println("fail");
}
