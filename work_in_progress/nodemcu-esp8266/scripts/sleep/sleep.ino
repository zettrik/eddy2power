#include <ESP8266WiFi.h>

const char* ssid = "your wifi ssid";
const char* password = "your wifi pw";
const int sleepTime = 10;       // deactivate wifi for ... seconds
const int led1 = 16;            // built in LED and D0
IPAddress local_ip(127,0,0,0);  // will be set via dhcp

int count = 4;
int ms = 250;

void setup() {
  Serial.begin(115200);
  Serial.println();
  pinMode(led1, OUTPUT);
}

void setup_wifi(){
  WiFi.begin(ssid, password);
  WiFi.mode(WIFI_STA);
  Serial.println("Waiting for wlan connection");
  while(WiFi.status() != WL_CONNECTED) {
    // blink while connecting
    blink(1, 125);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  local_ip = WiFi.localIP();
  Serial.println(local_ip);
  blink(10, 25);
}

void wifi_sleep() {
  Serial.println("Wifi goes to sleep.");
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin(sleepTime * 1000000L);
  Serial.println("Wifi is sleeping, wait some seconds.");
  delay(sleepTime * 1000); //Hang out at 15mA
  WiFi.mode(WIFI_STA);
}

void deep_sleep() { 
  Serial.println("Node takes a nap.");
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  //Serial.end();
  ESP.deepSleep(3000); 
  delay(100);
  blink(3, 1000);
}

void blink(int count, int ms) {
  for (int i = 0; i < count; i++) {
    // blink fast when successful
    digitalWrite(led1, 0);
    delay(ms);
    digitalWrite(led1, 1);
    delay(ms);
  } 
}

void loop() {
  Serial.print("Wifi status: ");
  Serial.println(WiFi.status());
  setup_wifi();
  Serial.print("Wifi status: ");
  Serial.println(WiFi.status());
  blink(3, 1000);
  wifi_sleep();
  Serial.print("Wifi status: ");
  Serial.println(WiFi.status());
  delay(sleepTime * 1000);
  blink(3, 1000);
  //deep_sleep();
  //Serial.begin(115200);
  blink(5, 1000);
}

