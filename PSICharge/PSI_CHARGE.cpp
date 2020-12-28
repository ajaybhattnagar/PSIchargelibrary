#include "PSI_CHARGE.h"

#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

Pwm::Pwm(bool temp) {
  //setup the PWM by default on pin 12 (timer one)
#define TOP ((F_CPU / 2000000) * 1000) // for 1KHz (=1000us period)
  pinMode(12, OUTPUT);
}

void Pwm::setPWM(int amps) {
  //deifne the amps and conver them to duty cycle.
  amperage = amps;
  int ocr1b = 0;
  if ((amperage >= 6) && (amperage <= 51)) {
    ocr1b = 25 * amperage / 6 - 1;  // J1772 states "Available current = (duty cycle %) X 0.6"
  } else if ((amperage > 51) && (amperage <= 80)) {
    ocr1b = amperage + 159;  // J1772 states "Available current = (duty cycle % - 64) X 2.5"
  }
  else {
    TCCR1A = 0; //error: PWM turned off
    Serial.println("Invalid Duty Cycle!!"); ///error management API here
  }
  TCCR1A = _BV(COM1A0) | _BV(COM1B1) | _BV(WGM11) | _BV(WGM10);
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS11) | _BV(CS10);
  OCR1A = 249;
  OCR1B = ocr1b;

}
