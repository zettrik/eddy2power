#include <ESP8266WiFi.h>
#include "credentials.h" //Contains WiFi SSID and password

const int sleepTime = 6; //Power down WiFi for 6 seconds

void setup()
{
  Serial.begin(115200);
}

void loop()
{
  WiFi.begin(ssid, pass); //Connect to local Wifi

  Serial.println();
  Serial.print("Connecting to WiFi");
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("WiFi Connected!");

  //Do your thing.

  //Time to let the WiFi go to sleep!
  Serial.println("Sleeping...");
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin(sleepTime * 1000000L); //In uS. Must be same length as your delay
  delay(sleepTime * 1000); //Hang out at 15mA for 6 seconds
  WiFi.mode(WIFI_STA);
  Serial.println("Awake!");
}

