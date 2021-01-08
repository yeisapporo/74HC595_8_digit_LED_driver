// sample application program for the driver.
// minimum ntp clock

#include "hc595led.h"

#include <SPI.h>
#include <Arduino.h>
#include <FreeRTOS.h>

#include <WiFi.h>
#include <WebServer.h>

void ntpTest(const char *srv) {
  configTime(9 * 3600, 0, srv);
  struct tm timeInfo;
  getLocalTime(&timeInfo);
  Serial.printf(" %04d/%02d/%02d %02d:%02d:%02d\n",
          timeInfo.tm_year + 1900, timeInfo.tm_mon + 1, timeInfo.tm_mday,
          timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);
}

HC595LED led = HC595LED(17, 21, 22);

void setup(void) {
  Serial.begin(115200);

  WiFi.begin("rs500m-14aaf8-1", "6a6a227fb3f17");
  while(WiFi.status() != WL_CONNECTED) {
    delay(250);
  }
  Serial.print("IP Addr. : ");
  Serial.println(WiFi.localIP());

  ntpTest("time.google.com");

  Serial.println("Initialized");
}


void loop() {
  struct tm timeInfo;
  getLocalTime(&timeInfo);

  char buf[17] = {0};

  sprintf(buf, "%02d:%02d:%02d", timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);
  led.setToLED(buf);

  if(timeInfo.tm_hour == 3 && timeInfo.tm_min == 0 && timeInfo.tm_sec == 0) {
      ntpTest("time.google.com");
      delay(1);
  } else {
    delay(500);
  }  
}
