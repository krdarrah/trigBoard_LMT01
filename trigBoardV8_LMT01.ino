/*
   Using trigBoard with LMT01 temperature sensor and sending on time interval to grafana using Corlysis


*/
//TIP to delete influx database data - DROP SERIES FROM LMT01

#include "includes.h"
#include <HTTPClient.h>// needed this for Corlysis
const char fwVersion[] = "8/05/20_LMT01";

char temperatureString[5];//where the temperature in F/C is stored
boolean getLMT01();

// USE CONFIGURATOR TO SET EVERYTHING UP
// Database = trigBoard Name
// Measurement = Message when contact opens
// location = Message when contact closes
// C or F in = Message when Button Pressed
// Token = Contact Still Open

void setup() {
  pinMode(ESPlatchPin, OUTPUT);
  digitalWrite(ESPlatchPin, HIGH);
  pinMode(LEDpin, OUTPUT);
  Serial.begin(115200);
  checkWakeupPins();
  loadConfiguration(filename, config);
  rtcInit(config.timerCountDown, false);
  Serial.println(getBattery(), 2);

  if (getLMT01()) {

    // ONLY Sending data to Corlysis
    //  if (pushLogic()) { //decide if push will occur or nt and what message will be
    //    if (wiFiNeeded) {
    if (connectWiFi()) {
      sendToCorlysis();
      //        pushOver();
      //        pushSafer();
      //        ifttt();
      //        mqtt();
    }
    //    }
    //    udp();
    //  }
  }
  killPower();
  waitForButton();
  initBluetooth();
}

void loop() {
  if (!OTAsetup)
    serviceBluetooth();
  else
    checkOTA();
}
