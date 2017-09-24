#include <ESP8266WiFi.h>

const char* ssid = "";
const char* password = "";
const int sleepTime = 60;       // deactivate wifi for ... seconds
const int led1 = 16;            // built in LED and D0
IPAddress local_ip(127,0,0,0);  // will be set via dhcp


void setup() {
  Serial.begin(115200);
  Serial.println();
  pinMode(led1, OUTPUT);
}

void setup_wifi(){
  WiFi.begin(ssid, password);
  Serial.println("Waiting for wlan connection");
  while(WiFi.status() != WL_CONNECTED) {
    // blink slowly while connecting
    digitalWrite(led1, 0);
    delay(125);
    digitalWrite(led1, 1);
    delay(125);
    digitalWrite(led1, 0);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  local_ip = WiFi.localIP();
  Serial.println(local_ip);
  for (int i = 0; i < 10; i++) {
    // blink fast when successful
    digitalWrite(led1, 0);
    delay(25);
    digitalWrite(led1, 1);
    delay(25);
  }
}
void wifi_sleep() {
  Serial.println("Wifi goes to sleep!");
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin(sleepTime * 1000000L);
  //Serial.println(WiFi.status());
  Serial.println("Wifi is sleeping, wait some seconds...!");
  delay(sleepTime * 1000); //Hang out at 15mA
  WiFi.mode(WIFI_STA);
  Serial.println(WiFi.status());
}

void loop() {
  setup_wifi();
  // instead of doing some tinhgs just wait
  delay(sleepTime * 1000);
  wifi_sleep();
}

