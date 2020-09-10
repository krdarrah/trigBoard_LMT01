

const int LMT01_Data_Pin = 13;//low side, where we catch the pulses
const int LMT01_Power_Pin = 12;
volatile uint16_t pulseCount = 0;

void IRAM_ATTR pulseCounter() {
  pulseCount++;
}

void getPulses() {
  pinMode(LMT01_Power_Pin, OUTPUT);
  digitalWrite(LMT01_Power_Pin, HIGH);
  pinMode(LMT01_Data_Pin, INPUT_PULLUP);
  attachInterrupt(LMT01_Data_Pin, pulseCounter, FALLING);
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
  detachInterrupt(LMT01_Data_Pin);
  digitalWrite(LMT01_Power_Pin, LOW);
  pinMode(LMT01_Data_Pin, INPUT);
}



boolean getLMT01() {
  unsigned long getLMT01startTime = millis();
  int lastPulseCount = 0;
  int goodPulsesInRow = 0;
  for (int i = 0; i < 20; i++) {
    getPulses();
    Serial.print("pulse=");
    Serial.print(pulseCount);
    Serial.print(" vs last ");
    Serial.println(lastPulseCount);
    if (pulseCount < (lastPulseCount + 10) && pulseCount  > (lastPulseCount - 10)) { //
      goodPulsesInRow++;
      if (goodPulsesInRow == 5) {
        Serial.println("Found 5 in a row!");
        break;
      }
    }
    else if (i > 0) {//just since it would fail on first reading
      goodPulsesInRow = 0;
      Serial.println("FAIL!! starting count over");
      delay(100);//extra delay to reset things
    }
    lastPulseCount = pulseCount;
    if (i == 19) {//all the way through and never found 5 good ones in a row
      Serial.println("Could not find temp");
      delay(10);
      return false;
    }
  }

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
  Serial.print("ms to measure ");
  Serial.println(millis() - getLMT01startTime);
  return true;
}
