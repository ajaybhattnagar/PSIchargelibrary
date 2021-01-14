#include "PSI_CHARGE.h"

#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

////////////////////////////////PWM/////////////////////////////////////////////////
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
/////////////////////////////////PWM END////////////////////////////////////////////////


/////////////////////////////////L1 L2 SENSE////////////////////////////////////////////////
L1L2Sense::L1L2Sense(int L1, int L2) {
  L1pin = L1;
  L2pin = L2;
  pinMode(L1pin, INPUT);
  pinMode(L2pin, INPUT);
}

int L1L2Sense::check() {
  int L1value = digitalRead(L1pin);
  int L2value = digitalRead(L2pin);

  if (L1value && L2value) {
    return 0; //both low BAD GROUND
  }
  if (!L1value && L2value) {
    return 3;  //L2
  }
  if (L1value && !L2value) {
    return 2; //L1
  }
  if (!L1value && !L2value) {
    return 1; // both HIGH
  }
}

/////////////////////////////////L1 L2 SENSE END////////////////////////////////////////////////

/////////////////////////////////CT SELF TEST////////////////////////////////////////////////

CTselfTest ::CTselfTest(int temp_CTPwmPin, int temp_CTReadPin) {
  CTPwmPin = temp_CTPwmPin;
  CTReadPin = temp_CTReadPin;
  pinMode(CTPwmPin, OUTPUT);
  pinMode(CTReadPin, INPUT);
}

int CTselfTest::check() {
  TCCR4B = TCCR4B & B11111000 | B00000011;  //has to be pin 6
  analogWrite(CTPwmPin, 128 );  //50% on off pwm
  delay(1000);
  int selfTest = digitalRead(CTReadPin);
  if (selfTest) {
    Serial.println("CT self test passed");
    TCCR4B = 0;
    return 0;
  }
  if (!selfTest) {
    Serial.println("CT self test failed");
    return 1;
  }
}

void CTselfTest::errorLights() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}

/////////////////////////////////CT SELF TEST END////////////////////////////////////////////////

/////////////////////////////////GFCI TEST////////////////////////////////////////////////
GFCITest::GFCITest(int temp_GFCIReadPin) {
  GFCIReadPin =  temp_GFCIReadPin;
  pinMode(GFCIReadPin, INPUT);
}

int GFCITest::check() {
  int GFCITest = digitalRead(GFCIReadPin);
  if (GFCITest) {
    Serial.println("GFCI failed");
    return 1;
  }
  if (!GFCITest) {
    Serial.println("GFCI passed");
    return 0;
  }
}

void GFCITest::errorLights() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(250);
  digitalWrite(LED_BUILTIN, LOW);
  delay(250);
}

/////////////////////////////////GFCI END////////////////////////////////////////////////


/////////////////////////////////LED LIGHT////////////////////////////////////////////////
ledLights::ledLights(int temp_redPin, int temp_greenPin, int temp_bluePin) {
  redPin = temp_redPin;
  greenPin = temp_greenPin;
  bluePin = temp_bluePin;
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, INPUT);
  pinMode(bluePin, OUTPUT);
}

void ledLights::setColor(int red, int green, int blue) {
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}

/////////////////////////////////LED LIGHTS END////////////////////////////////////////////////

/////////////////////////////////Ammeter////////////////////////////////////////////////
ammeter::ammeter(int temp_readPin, int temp_voltageDivide, int temp_offSet) {
  readPin = temp_readPin;
  voltageDivide = temp_voltageDivide;
  offSet = temp_offSet;

}

float ammeter::readAmps() {
#define MA_PTS 32 // # points in moving average MUST BE power of 2
#define MA_BITS 5 // log2(MA_PTS)

  int CURRENT_SAMPLE_INTERVAL = 35;
  int CURRENT_ZERO_DEBOUNCE_INTERVAL = 5;
  long m_AmmeterReading = 0.00;

  //offSet = 1;

  unsigned long sum = 0.00;
  unsigned int zero_crossings = 0;
  long sample = 0.00;
  unsigned long last_zero_crossing_time = 0, now_ms;
  long last_sample = -1; // should be impossible - the A/d is 0 to 1023.
  unsigned int sample_count = 0;
  for (unsigned long start = millis(); ((now_ms = millis()) - start) < CURRENT_SAMPLE_INTERVAL; ) {
    sample =  (long)analogRead(readPin);
    // If this isn't the first sample, and if the sign of the value differs from the
    // sign of the previous value, then count that as a zero crossing.
    // 522 shall be changed based on the nominal voltage
    //in order to find 522: scope the output. 2.54*512/2.5.
    if (last_sample != -1 && ((last_sample > voltageDivide) != (sample > voltageDivide))) {
      // Once we've seen a zero crossing, don't look for one for a little bit.
      // It's possible that a little noise near zero could cause a two-sample
      // inversion.
      if ((now_ms - last_zero_crossing_time) > CURRENT_ZERO_DEBOUNCE_INTERVAL) {
        zero_crossings++;
        last_zero_crossing_time = now_ms;
      }
    }
    last_sample = sample;
    switch (zero_crossings) {
      case 0:
        continue; // Still waiting to start sampling
      case 1:
      case 2:
        // Gather the sum-of-the-squares and count how many samples we've collected.
        //522 shall be changes based on the nominal voltage.
        sum += (unsigned long)((sample - voltageDivide) * (sample - voltageDivide));
        sample_count++;
        continue;
      case 3:
        // The answer is the square root of the mean of the squares.
        // But additionally, that value must be scaled to a real current value.
        // we will do that elsewhere


        m_AmmeterReading = sqrt(sum / sample_count)  ;
        //Serial.println(m_AmmeterReading);

        ////offsett the values based on amps.
/*
        if (m_AmmeterReading > 13) {
          if ((m_AmmeterReading > 30)&&(m_AmmeterReading < 59)) {
            m_AmmeterReading += 3;
          } if ((m_AmmeterReading > 60)&&(m_AmmeterReading < 90)) {
            m_AmmeterReading += 2;
          } if ((m_AmmeterReading > 91)&&(m_AmmeterReading < 150)) {
            m_AmmeterReading += 1;
          } if ((m_AmmeterReading > 151)&&(m_AmmeterReading < 210)) {
            m_AmmeterReading = m_AmmeterReading;
          } if ((m_AmmeterReading > 211)&&(m_AmmeterReading < 300)) {
            m_AmmeterReading -=2;
          }
          
        }
*/
      m_AmmeterReading = (m_AmmeterReading*0.954 + offSet) ;
        return m_AmmeterReading;
    }
  }
  // ran out of time. Assume that it's simply not oscillating any.
  m_AmmeterReading = 0;

}

/////////////////////////////////Ammeter////////////////////////////////////////////////
