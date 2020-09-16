

const int LMT01_Data_Pin = 13;//low side, where we catch the pulses
const int LMT01_Power_Pin = 12;
volatile uint16_t pulseCount = 0;

void IRAM_ATTR pulseCounter() {
  pulseCount++;
}

void getPulses() {
  //unsigned long debugStartTime = micros();
  pulseCount = 0;
  pinMode(LMT01_Power_Pin, OUTPUT);
  digitalWrite(LMT01_Power_Pin, HIGH);
  pinMode(LMT01_Data_Pin, INPUT_PULLUP);
  attachInterrupt(LMT01_Data_Pin, pulseCounter, FALLING);
  unsigned long startTime = micros();
  //LMT01 running at ~88kHz, so would be 12us a period, 10ms should be plenty of quiet time, plus it takes ~54ms to bootup
  while (micros() - startTime < 10000) {//just looking for some quiet time
    if (pulseCount != 0) {
      pulseCount = 0;
      startTime = micros();
    }
  }
//  Serial.println(micros() - debugStartTime);
//  debugStartTime = micros();
  while (pulseCount == 0) {
    //waiting for first pulse
  }
  //data transmission start
  startTime = micros();//
  int oldPulseCount = 0;
  //let's give it a full 30ms to setle out before we bail
  while (micros() - startTime < 30000) {
    if (oldPulseCount != pulseCount) {
      oldPulseCount = pulseCount;
      startTime = micros();
    }
  }
  //  Serial.println(micros() - debugStartTime);
  //  debugStartTime = micros();
  detachInterrupt(LMT01_Data_Pin);
  digitalWrite(LMT01_Power_Pin, LOW);
  pinMode(LMT01_Data_Pin, OUTPUT);
  digitalWrite(LMT01_Data_Pin, LOW);
  delay(1);
}



boolean getLMT01() {
  //  WiFi.mode(WIFI_OFF);
  //  btStop();
  unsigned long getLMT01startTime = millis();
  float lastTemperature = 0.00;
  int goodTempsInRow = 0;
  float tempC = 0.00;
  float tempF = 0.00;

  int rollingTemperIndex = 0;
  const int numberInArow = 10;
  float rollingTemperArray[numberInArow];
  const int totalTries = 100;
  for (int i = 0; i < totalTries; i++) {
    getPulses();

    tempC = (pulseCount * 256.00 / 4096.00) - 50.00;
    Serial.print("temp=");
    Serial.println(tempC, 2);

    rollingTemperArray[rollingTemperIndex] = tempC;
    rollingTemperIndex++;
    if (rollingTemperIndex == numberInArow)
      rollingTemperIndex = 0;

    bool entireArrayGood = true;
    //re-test entire array for consistency
    for (int j = 0; j < numberInArow; j++) {
      for (int k = 0; k < numberInArow; k++) {
        //        Serial.print(rollingTemperArray[j], 2);
        //        Serial.print(" vs ");
        //        Serial.print(rollingTemperArray[k], 2);
        if (rollingTemperArray[j] > (rollingTemperArray[k] + 1.0) || rollingTemperArray[j]  < (rollingTemperArray[k] - 1.0)) { //testing
          //outside range
          entireArrayGood = false;

          Serial.println("X");
          break;
        } else {
          Serial.print("$");
        }
      }
      if (!entireArrayGood)
        break;
    }
    if (entireArrayGood) { //means it was never flipped to false, all 10 readings are within eachother
      Serial.println("ALL ARE GOOD!");
      break;
    }

    if (i == totalTries - 1) { //all the way through and never found 5 good ones in a row
      Serial.println("Could not find temp");
      delay(10);
      return false;
    }
  }

  //might as well take an average
  tempC = 0;
  for (int i = 0; i < 10; i++) {
    tempC = rollingTemperArray[i] + tempC;
  }
  tempC = tempC / 10.00;


  tempF = tempC * 9.00 / 5.00 + 32.00;
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
