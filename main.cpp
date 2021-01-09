/////////////////////////////////////////////////////////////
// all you have to do is to include the header file below!
/////////////////////////////////////////////////////////////
#include "hc595led.h"

#include <Arduino.h>
#include <FreeRTOS.h>
#include <WiFi.h>

void ntpTest(const char *srv) {
  configTime(9 * 3600, 0, srv);
  struct tm timeInfo;
  getLocalTime(&timeInfo);
  Serial.printf(" %04d/%02d/%02d %02d:%02d:%02d\n",
          timeInfo.tm_year + 1900, timeInfo.tm_mon + 1, timeInfo.tm_mday,
          timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);
}

// declare the global instance. 
HC595LED led = HC595LED(17,   // SCK
                        21,   // DIO
                        22);  // RCK

void setup(void) {
  Serial.begin(115200);

  WiFi.begin("rs500m-14aaf8-1", "6a6a227fb3f17");
  while(WiFi.status() != WL_CONNECTED) {
    delay(250);
  }
  Serial.print("IP Addr. : ");
  Serial.println(WiFi.localIP());

  ntpTest("time.google.com");

  Serial.println("set-up done.");
}

void loop() {
  static char buf[20] = {0};
  struct tm timeInfo;

  getLocalTime(&timeInfo);
  sprintf(buf, "Jp%02d.%02d.%02d", timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);
  led.setToLED(buf);

  // once a day at 3:59:59,
  if(timeInfo.tm_hour == 3 && timeInfo.tm_min == 59 && timeInfo.tm_sec == 59) {
      // adjust time
      led.setToLED("-adjust-");
      ntpTest("time.google.com");
      delay(1000);
  } else {
    delay(500);
  }  
}
