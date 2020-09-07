
void sendToCorlysis() {
  HTTPClient http;
  char corlysisUrl[200];
  sprintf(corlysisUrl, "http://corlysis.com:8087/write?db=%s&u=token&p=%s", config.trigName, config.StillOpenMessage);
  http.begin(corlysisUrl);

  //HTTPS variant - check ssh public key fingerprint
  //sprintf(corlysisUrl, "https://corlysis.com:8086/write?db=%s&u=token&p=%s", dbName, dbPassword);
  //http.begin(corlysisUrl, "FF:2D:E9:25:75:39:D1:A0:5C:99:02:34:EF:81:73:0F:3F:3E:2D:0D");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  char payload[200];
  //LMT01 temperature=100,battery=4,location="front"
  sprintf(payload, "%s temperature=%s,battery=%s,location=\"%s\"", config.triggerOpensMessage, temperatureString, batCharString, config.triggerClosesMessage);
  int httpCode = http.POST(payload);
  Serial.print("http result:");
  Serial.println(httpCode);
  //http.writeToStream(&Serial);//seemed to slow things down... 
  http.end();
  //  if (httpCode == 204) {
  //    Serial.println("Data successfully sent.");
  //  }
}
