

const int LMT01_Data_Pin = 13;//low side, where we catch the pulses with the ADC
volatile uint16_t pulseCount = 0;


void getPulses() {
  //Serial.println("temp configured");
  //wait for 54ms timeout
  pulseCount = 0;
  unsigned long startTime = millis();
  while (millis() - startTime < 50) {
    if (pulseCount != 0) {
      pulseCount = 0;
      startTime = millis();
    }
  }
  pulseCount = 0;
  //data transmission start
  startTime = millis();//t
  int oldPulseCount = 0;
  while (millis() - startTime < 50) {
    if (oldPulseCount != pulseCount) {
      oldPulseCount = pulseCount;
      startTime = millis();
    }

  }

}

void IRAM_ATTR pulseCounter() {
  pulseCount++;
}

boolean getLMT01() {
  pinMode(LMT01_Data_Pin, INPUT_PULLUP);
  attachInterrupt(LMT01_Data_Pin, pulseCounter, FALLING);

  int lastPulseCount = 0;
  int goodPulsesInRow = 0;
  for (int i = 0; i < 20; i++) {//gonna try 20 times until we hit 5 in a row
    getPulses();
    Serial.print("pulse=");
    Serial.println(pulseCount);
    if (pulseCount < (lastPulseCount + 10) && pulseCount  > (lastPulseCount - 10)) { //if temperature reading looks strange, keep going  till you get two in a row
      goodPulsesInRow++;
      if (goodPulsesInRow == 5) {
        Serial.println("Found 5 in a row!");
        break;
      }
    }
    else if (i > 0) {//just since it would fail on first reading
      goodPulsesInRow = 0;
      Serial.println("FAIL!! starting count over");
    }
    lastPulseCount = pulseCount;
    if (i == 19) {//all the way through and never found 5 good ones in a row
      Serial.println("Could not find temp");
      delay(10);
      return false;
    }
  }
  detachInterrupt(LMT01_Data_Pin);
  pinMode(LMT01_Data_Pin, INPUT);


  float tempC = (pulseCount * 256.00 / 4096.00) - 50.00;
  float tempF = tempC * 9.00 / 5.00 + 32.00;
  Serial.print("tempC=");
  Serial.print(tempC, 2);
  Serial.print("  tempF=");
  Serial.println(tempF, 2);

  if (strcmp(config.buttonMessage, "C") == 0) {
    Serial.println("Sending  in C");
    dtostrf(tempC, 1, 2, temperatureString);//convert float to str
  } else { //in F
    Serial.println("Sending  in F");
    dtostrf(tempF, 1, 2, temperatureString);//convert float to str
  }
  return true;
}
