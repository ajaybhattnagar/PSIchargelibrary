#include <PSI_CHARGE.h>

// declare pins for CT self test
CTselfTest CTself(6,7);

// Declare pin for GFCI 
GFCITest GFCI(8);


void setup() {
  //put your setup code here, to run once:
  Serial.begin(9600);


  pinMode(LED_BUILTIN, OUTPUT);

// loop for CT self test
 while(CTself.check()){
    CTself.errorLights();
  }

// loop for GFCI test
 while(GFCI.check()){
    GFCI.errorLights();
  }
  
  }
  
void loop() {
  

 
  
}
