/*
 * blinking onboard LED means:
 *   fast  - connecting to wireless network
 *   slow  - connecting to mqtt broker
 *   flash - sending bytes via mqtt
 *   on    - critical power supply
 * 
 * mqtt channels in use:
 *   nodes - status messages (button1, button2, analog_in, time)
 *   node1-button1 - message with state imediatly after a change (bool button1)
 *   node1-button2 - message with state imediatly after a change (bool button2)
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"


const char* ssid = "";
const char* password = "";
const char* mqtt_server = "172.16.0.1";
const int mqtt_port = 1883;
const int button1 = 5; // nodemcu pin: D1
const int button2 = 4; // nodemcu pin: D2
const int led1 = 16;   // built in LED and D0
const int led2 =  0;   // nodemcu pin: D4
const int chipSelect = 15;
const int sleepTime = 9; // sleep time between readings in seconds

int led1_state = 0;
int led2_state = 0;
int button1_state = 0;
int button2_state = 0;
int button1_state_last = 0;
int button2_state_last = 0;
int analog_in = 0;
int analog_in_sum = 0;
unsigned long now = 0;
unsigned long before = 0;
char mqtt_message[120];
char sd_data;

ADC_MODE(ADC_VCC); //read ESP voltage instead of analog_in A0, leave A0 unconnected
IPAddress local_ip(127,0,0,0); // will be set via dhcp
WiFiClient espClient;
PubSubClient client(espClient);
Sd2Card card;
SdVolume volume;
SdFile root;
RTC_DS1307 rtc;
DateTime rtc_time;

void setup() {
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  digitalWrite(led1, 1);
  digitalWrite(led2, 1);
  Serial.begin(115200);
  setup_wifi();
  setup_mqtt();
  setup_sd();
  setup_rtc();
}

void setup_rtc(){
  Wire.begin(5, 4);  // SDA, SCL
    
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
  }
  else {
    Serial.println("RTC connected");
  }
  
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    //rtc.adjust(DateTime(2017, 9, 25, 21, 49, 0));
  }
  //rtc.adjust(DateTime(2017, 9, 10, 10, 02, 23));
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}  

void print_rtc() {
  rtc_time = rtc.now();
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
  }
  
  Serial.println(rtc_time.unixtime());

  Serial.print(rtc_time.year(), DEC);
    Serial.print('-');
    Serial.print(rtc_time.month(), DEC);
    Serial.print('-');
    Serial.print(rtc_time.day(), DEC);
    Serial.print(' ');
    Serial.print(rtc_time.hour(), DEC);
    Serial.print(':');
    Serial.print(rtc_time.minute(), DEC);
    Serial.print(':');
    Serial.print(rtc_time.second(), DEC);
    Serial.println();
}


void setup_sd(){
  if (!SD.begin(chipSelect)) {
    Serial.println("SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialized");
  
  if (SD.exists("datalog.txt")) {
    Serial.println("Found file datalog.txt on card");
  }
  else {
    Serial.println("File datalog.txt doesn't exist on card");
  }
}

void setup_mqtt(){
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void setup_wifi() {
  WiFi.begin(ssid, password);
  Serial.println("Starting wifi connection");
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

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
    
void mqtt_pub(char* channel, char* mqtt_message) {
    Serial.print(channel);
    Serial.print(": ");
    Serial.println(mqtt_message);
    //Serial.flush();
    client.publish(channel, mqtt_message);
    digitalWrite(led1, 0);
    delay(1);
    digitalWrite(led1, 1);
    // wait a moment to ignore button bouncing & prevent fast polling
    delay(10);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("node1")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("nodes", "node1 says hello!");        
      // ... and resubscribe to inbound messages
      //client.subscribe("node_in");
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 3 seconds");
      // blink'n wait 3 seconds before retrying
      digitalWrite(led1, 0);
      delay(1500);
      digitalWrite(led1, 1);
      delay(1500);
    }
  }
}

void check_battery() {
  /* these values were measured with 330kOhm:22kOhm voltage divider at A0
   * raw input  :: voltage divider :: ADC analog_in
   * 3.4V   ::  206mV   ::   51
   * 3.3V   ::  190mV   ::   48
   * 2.8V   ::  170mv   ::   44
   * todo: check
   *    high 3x 4.2V 12.6V
   *    low 3x 2,8V 8,4V
  */
  
  //analog_in = analogRead(A0);
  analog_in = multiple_read(9);
  snprintf(mqtt_message, 75, "node1: Input power: %04d, %ld", analog_in, now);
  mqtt_pub("nodes", mqtt_message);
  Serial.println(analog_in);

  return;
  
  // warn about high power and wait a second
  if (analog_in >= 51) {
    Serial.println("Input power is too high!");
    snprintf(mqtt_message, 75, "node1: Input power is too high! %04d, %ld", analog_in, now);
    mqtt_pub("nodes", mqtt_message);
    digitalWrite(led1, 0);
    delay(1000);
    digitalWrite(led1, 1);
  }
  // warn about low power
  // very low voltage probably means we are running by usb
  else if (analog_in <= 41) {
    Serial.println("Input power is too low!");
    snprintf(mqtt_message, 75, "node1: Input power is too low! %04d, %ld", analog_in, now);
    mqtt_pub("nodes", mqtt_message);
    digitalWrite(led1, 0);
    delay(1000);
    digitalWrite(led1, 1);
  }
}

float multiple_read(int times) {
  analog_in_sum = 0;
  for(int i = 0; i < times; i++) {
    delay(9);
    //analog_in_sum += analogRead(A0); // one read lasts about 100µs
    analog_in_sum += ESP.getVcc();
    }
  analog_in_sum /= times;
  return analog_in_sum;
}

void read_sd() {
  if (SD.exists("datalog.txt")) {
    Serial.println("datalog.txt exists.");
    snprintf(mqtt_message, 100, "node1: datalog.txt exists %ld", now);
    mqtt_pub("nodes", mqtt_message);
    //try to open and read it
    File dataFile = SD.open("datalog.txt");
    if (dataFile) {
      while (dataFile.available()) {
        sd_data = dataFile.read();
        Serial.write(sd_data);
        snprintf(mqtt_message, 75, "node1: sd_data... ,%ld", now);
        mqtt_pub("nodes", mqtt_message);
        //mqtt_pub("nodes", sd_data);
      }
      dataFile.close();
    }
    // if the file isn't open, pop up an error:
    else {
      Serial.println("error opening datalog.txt");
      snprintf(mqtt_message, 100, "node1: could not read datalog.txt %ld", now);
      mqtt_pub("nodes", mqtt_message);
    }
  }

  else {
    Serial.println("datalog.txt doesn't exist.");
    snprintf(mqtt_message, 100, "node1: datalog.txt does not exist %ld", now);
    mqtt_pub("nodes", mqtt_message);
  }
}

void write_sd() {
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  if (dataFile) {
    analog_in = multiple_read(5);
    snprintf(mqtt_message, 100, "node1 writes to sd: %04d,%ld", analog_in, now);
    mqtt_pub("nodes", mqtt_message);
    dataFile.println(analog_in);
    dataFile.close();
    Serial.println("Data written to datalog.txt");
  }
  else {
    Serial.println("Error opening datalog.txt");
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
  //Serial.println(WiFi.status());
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  button1_state = digitalRead(button1);
  button2_state = digitalRead(button2);
  // keep in mind: millis() will start again from 0 about every 50 days
  now = millis();

  print_rtc();
  
  if ((unsigned long)(now - before) >= 10000) {
    check_battery();
    before = now;
    if (SD.exists("datalog.txt")) {
      Serial.println("File datalog.txt exists on card");
      snprintf(mqtt_message, 75, "node1: %01d,%01d,%04d,%ld", button1_state, button2_state, analog_in, now);
      mqtt_pub("nodes", mqtt_message);
    }
    //write_sd();
  }
  if (button1_state != button1_state_last) {
    button1_state_last = button1_state;
    snprintf(mqtt_message, 40, "%01d", button1_state);
    mqtt_pub("/node/1/button/1", mqtt_message);
  }
  else if (button2_state != button2_state_last) {
    button2_state_last = button2_state;
    snprintf(mqtt_message, 40, "%01d", button2_state);
    mqtt_pub("/node/1/button/2", mqtt_message);
  }

  delay(sleepTime * 1000);
  write_sd();
  //wifi_sleep();
  
  //todo: setting up wifi again does not work properly for now just do a soft reset 
  //setup_wifi();
  //todo:  real deep sleep also does not work atm
  //ESP.deepSleep(sleepTime * 1000);
  //ESP.reset();
}
