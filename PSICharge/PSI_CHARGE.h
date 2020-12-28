
#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif



class Pwm {

  private:
    int amperage;


  public:

    Pwm(bool temp);
    void setPWM(int amps);

};
