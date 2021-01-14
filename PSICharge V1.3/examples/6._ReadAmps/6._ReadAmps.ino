// EmonLibrary examples openenergymonitor.org, Licence GNU GPL V3

#include <PSI_CHARGE.h>

float wattsPerHour = 0;
float ammeterOffset = 4;
float wattsPerSecond = 0;
float preWattsPerSecond = 0;
float totalConsumed = 0;
int count = 0;

//ammeter amps(A1,522, 0);
ammeter amps(A1,522, 2); /// SCT-013  

void setup()
{
  Serial.begin(9600);
}

void loop()
{ 
 
 float amp = amps.readAmps();
 
 if(amp >= 10){
  float wattsPerSecond = amp*118/10; //power
  wattsPerSecond = wattsPerSecond + preWattsPerSecond ;
  preWattsPerSecond = wattsPerSecond;
  count++;
  Serial.print("Amp: ");
  Serial.println(amp/10);
  Serial.print("Total Consumed: ");
  Serial.println(wattsPerSecond);
  Serial.print("Total time: ");
  Serial.println(count);
  Serial.println("");
 }
 delay(1000);

}
