/*
 * blinking onboard LED means:
 *   fast  - connecting to wireless network
 *   slow  - connecting to mqtt broker
 *   flash - sending bytes via mqtt
 *   on    - critical power supply
 * 
 * mqtt channel scheme:
 *   /node/X/sensor - publish from node X the value(s) of corresponding sensor (e.g. adc, mcp, internal vcc)
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_ADS1015.h>
#include <MCP3208.h>

/*
 * Change the following values to fit your setup.
 */
// wifi:
const char* ssid = "";
const char* password = "";

// mqtt broker:
const int node_id = 5;
const String mqtt_channel = "node/5/";
//const char* mqtt_server = "192.168.1.111";
const char* mqtt_server = "172.16.1.112";
const int mqtt_port = 1883;

// sensor measurement interval
const int sleep_time = 10;  // sleep time between readings in seconds

// buttons, led ... wiring and mapping to pins:
const int button1 = 4;     // nodemcu pin: D2
const int button2 = 0;     // nodemcu pin: D3
const int led1 = 16;       // nodemcu pin: D0 (and built in LED)
const int led2 =  5;       // nodemcu pin: D1
const int chipSelect = 10; // nodemcu pin: 
const int rtc_SDA = 2;     // nodemcu pin:
const int rtc_SDC = 9;     // nodemcu pin: 

// mcp3208 wiring:
#define CS_PIN 15    // nodemcu pin: D8
#define CLOCK_PIN 14 // nodemcu pin: D5
#define MOSI_PIN 13  // nodemcu pin: D7
#define MISO_PIN 12  // nodemcu pin: D6

// anemometer
const byte interruptPin1 = 4;  // nodemcu pin: D2
const byte interruptPin2 = 0;  // nodemcu pin: D3
volatile int interruptCounter1 = 0;
volatile int interruptCounter2 = 0;

/*
 * Don't change after here, unless you know what you do.
 */
int led1_state = 0;
int led2_state = 0;
int button1_state = 0;
int button2_state = 0;
int button1_state_last = 0;
int button2_state_last = 0;
int analog_in = 0;
int analog_in_sum = 0;
int val = 0;
unsigned long now = 0;
unsigned long before = 0;
unsigned long rtc_now;
unsigned long time_delta;
char mqtt_message[120];
char sd_data;
int adc0, adc1, adc2, adc3;
int mcp[8];

ADC_MODE(ADC_VCC); //read ESP voltage instead of analog_in A0, leave A0 unconnected
MCP3208 mcp3208(CLOCK_PIN, MOSI_PIN, MISO_PIN, CS_PIN);
IPAddress local_ip(127,0,0,0); // will be set via dhcp
WiFiClient espClient;
PubSubClient client(espClient);
Sd2Card card;
SdVolume volume;
SdFile root;
RTC_PCF8523 rtc;
DateTime rtc_time;
Adafruit_ADS1015 ads;

/*
 * bring everything up and running
 */
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
  setup_interrupts(); // for anemometer
  //setup_adc();
  //setup_sd();
  //setup_rtc();
}

/*
 * main loop
 */
void loop() {
  before = now;
  now = millis();
  // keep in mind: millis() will start again from 0 about every 50 days
  if (now < before) {
    before = now;
  }
  time_delta = now - before;
  
  Serial.print("uptime: ");
  Serial.println(now);
  Serial.print("time for last loop: ");
  Serial.println(time_delta);
  snprintf(mqtt_message, 100, "%ld,%ld", now, time_delta);
  mqtt_pub(mqtt_channel + "uptime", mqtt_message);
  
  read_mcp();
  check_battery();
  check_voltage();
   
  delay(sleep_time * 1000);
  
  check_interrupts();
}

/*
 * Spannungsschalter
 */
void check_voltage() {
  Serial.println(mcp[1]);
  if (mcp[1] > 1588) {
    Serial.println("high voltage");
    digitalWrite(led2, 1);
  }
  else {
    Serial.println("low voltage");
    digitalWrite(led2, 0);
  }
}

/*
 * Anemometer
 */
void setup_interrupts(){
  pinMode(interruptPin1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin1), handle_interrupt1, FALLING);
  pinMode(interruptPin2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin2), handle_interrupt2, FALLING);
}

void handle_interrupt1() {
  digitalWrite(led2, interruptCounter1 % 2);
  Serial.println(interruptCounter1 % 2);
  interruptCounter1++;
  //Serial.println(interruptCounter1);
}
void handle_interrupt2() {
  interruptCounter2++;
  //Serial.println(interruptCounter2);
}

void check_interrupts(){
  // interrupts since last loop
  //Serial.print("occured interrupts 1 in last loop: ");
  //Serial.println(interruptCounter1);
  //Serial.print("occured interrupts 2 in last loop: ");
  //Serial.println(interruptCounter2);
  snprintf(mqtt_message, 100, "%ld,%i,%i", time_delta, interruptCounter1, interruptCounter2);
  mqtt_pub(mqtt_channel + "interrupts", mqtt_message);
  // reset counter
  interruptCounter1 = 0;
  interruptCounter2 = 0;
}


/*
 * ADC
 */
void setup_adc() {
  /*
   * The ADC input range (or gain) can be changed via the following
   * functions, but be careful never to exceed VDD +0.3V max, or to
   * exceed the upper and lower limits if you adjust the input range!
   * Setting these values incorrectly may destroy your ADC!
   *                                                                ADS1015  ADS1115
   *                                                                -------  -------
   * ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
   * ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
   * ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
   * ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
   * ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
   * ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
  */
  ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV
  ads.begin();
  Serial.println("ADC initialized");
}

void read_adc() {
  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  adc2 = ads.readADC_SingleEnded(2);
  adc3 = ads.readADC_SingleEnded(3);
  Serial.print("AIN0: "); Serial.println(adc0);
  Serial.print("AIN1: "); Serial.println(adc1);
  Serial.print("AIN2: "); Serial.println(adc2);
  Serial.print("AIN3: "); Serial.println(adc3);
  Serial.println(" ");
}

/*
 * MCP3208
 */
void read_mcp() {
  /*
   * The MCP3208 has eight 12bit analog inputs.
   * We are reading them here one after another.
   * valid return values are between 0 - 4095
   * returns 8191 if no mcp3208 is present
   */
  //Serial.print("MCP3208 values:");
  for (int i=0; i<=7; i++) {
    mcp[i] = mcp3208.readADC(i);
    //Serial.print(mcp[i]);
    //Serial.print(", ");
  }
  //Serial.println("");

  snprintf(mqtt_message, 100, "%04i,%04i,%04i,%04i,%04i,%04i,%04i,%04i", mcp[0],  mcp[1], mcp[2], mcp[3], mcp[4], mcp[5], mcp[6], mcp[7]);
  mqtt_pub(mqtt_channel + "mcp3208", mqtt_message);
}

/*
 * real time clock
 */
void setup_rtc(){
  // set pin connection
  Wire.begin(rtc_SDA, rtc_SDC);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
  }
  else {
    Serial.println("RTC connected");
  }
  
  if (! rtc.initialized()) {
    Serial.println("RTC is not running!");
    // following line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    //rtc.adjust(DateTime(2017, 9, 25, 21, 49, 0));
  }
}  

unsigned long get_rtc() {
  rtc_time = rtc.now();
  if (! rtc.initialized()) {
    Serial.println("RTC is not running!");
    return 0;
  }
  else {
   return rtc_time.unixtime(); 
  }
  
  Serial.print("unixtimestamp: ");
  Serial.println(rtc_time.unixtime());
  Serial.print("date: ");
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

/*
 * SD Card
 */
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

void read_sd() {
  if (SD.exists("datalog.txt")) {
    Serial.println("datalog.txt exists.");
    snprintf(mqtt_message, 100, "node: datalog.txt exists %ld", now);
    mqtt_pub("nodes", mqtt_message);
    //try to open and read it
    File dataFile = SD.open("datalog.txt");
    if (dataFile) {
      while (dataFile.available()) {
        sd_data = dataFile.read();
        Serial.write(sd_data);
        snprintf(mqtt_message, 75, "node: sd_data... ,%ld", now);
        mqtt_pub("nodes", mqtt_message);
        //mqtt_pub("nodes", sd_data);
      }
      dataFile.close();
    }
    // if the file isn't open, pop up an error:
    else {
      Serial.println("error opening datalog.txt");
      snprintf(mqtt_message, 100, "node: could not read datalog.txt %ld", now);
      mqtt_pub("nodes", mqtt_message);
    }
  }

  else {
    Serial.println("datalog.txt doesn't exist.");
    snprintf(mqtt_message, 100, "node: datalog.txt does not exist %ld", now);
    mqtt_pub("nodes", mqtt_message);
  }
}

void write_sd() {
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  if (dataFile) {
    analog_in = multiple_read(5);
    snprintf(mqtt_message, 100, "node writes to sd: %04d,%ld", analog_in, now);
    mqtt_pub("nodes", mqtt_message);
    dataFile.println(analog_in);
    dataFile.close();
    Serial.println("Data written to datalog.txt");
  }
  else {
    Serial.println("Error opening datalog.txt");
  }
}

/*
 * wifi
 */
void setup_wifi() {
  WiFi.softAPdisconnect(true);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Starting wifi connection");
  // Try max. 20 seconds to connect to access point.
  //while(WiFi.status() != WL_CONNECTED) {
  for(int i = 0; i <= 79; i++) {
    // blink slowly while connecting
    digitalWrite(led1, 0);
    delay(125);
    digitalWrite(led1, 1);
    delay(125);
    digitalWrite(led1, 0);
    Serial.print(".");
    if(WiFi.status() == WL_CONNECTED) {
      i = 80;
    }
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

/*
 * putting wifi to sleep and waking up again does not work properly with mqtt 
 */
void wifi_sleep() {
  Serial.println("Wifi is going to sleep.");
  WiFi.softAPdisconnect();
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
    //WiFi.forceSleepBegin(sleepTime * 1000000L);
  WiFi.forceSleepBegin();
  delay(10);
  //Serial.println(WiFi.status());
  //wdt_reset();
  
}

void wifi_awake() {
  setup_wifi();
  Serial.println(WiFi.status());
}

/*
 * MQTT
 */
void setup_mqtt(){
  client.setServer(mqtt_server, mqtt_port);
}

void mqtt_reconnect() {
  client.disconnect();
  Serial.println("Connecting to MQTT broker.");
  // Try three times to reconnect to mqtt broker.
    //while (!client.connected()) {
  for(int i = 0; i <= 2; i++) {
    if (client.connect("node")) {
      Serial.println("Connection to MQTT broker is established.");
      client.publish("node", "hello!");
      client.loop();
      i = 3;
    }
    else {
      // retry mqtt connection three times, otherwise move on
      Serial.print("Conenction to MQTT borker failed. Client state: ");
      Serial.println(client.state());
      Serial.println("I will try again in 3 seconds.");
      // blink'n wait 3 seconds before retrying
      digitalWrite(led1, 0);
      delay(1500);
      digitalWrite(led1, 1);
      delay(1500);
    }
  }
  if (!client.connected()) {
    setup_wifi();
  }
}

void mqtt_check_connection() {

}
  
//void mqtt_pub(char* channel, char* mqtt_message) {
void mqtt_pub(String channel, char* mqtt_message) {
  if (!client.connected()){
    mqtt_reconnect();
  }
  else {
    char mqtt_channel[100];
    channel.toCharArray(mqtt_channel, 100);
    Serial.print(mqtt_channel);
    Serial.print(": ");
    Serial.println(mqtt_message);
    client.publish(mqtt_channel, mqtt_message);
    digitalWrite(led1, 0);
    delay(1);
    digitalWrite(led1, 1);
  }
}

/*
 * batteries; power connection of node
 */
void check_battery() {
  /* 
   * these values were measured with 330kOhm:22kOhm voltage divider at A0
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
  //Serial.print("VCC value: ");
  //Serial.println(analog_in);
  snprintf(mqtt_message, 100, "%04i", analog_in);
  mqtt_pub(mqtt_channel + "vcc", mqtt_message);

  return;
  
  // warn about high power and wait a second
  if (analog_in >= 51) {
    Serial.println("Input power is too high!");
    snprintf(mqtt_message, 75, "node %02d - Input power is too high! %04d, %ld", node_id, analog_in, now);
    mqtt_pub("nodes", mqtt_message);
    digitalWrite(led1, 0);
    delay(1000);
    digitalWrite(led1, 1);
  }
  // warn about low power
  // very low voltage probably means we are running by usb
  else if (analog_in <= 41) {
    Serial.println("Input power is too low!");
    snprintf(mqtt_message, 75, "node %02d - Input power is too low! %04d, %ld", node_id, analog_in, now);
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


/*
 * probably obsolete code...
 */
 
/*
  Serial.print("rtc: ");
  Serial.println(get_rtc());
*/

/*
  //read_adc();
  //button1_state = digitalRead(button1);
  //button2_state = digitalRead(button2);
*/

/*
  if ((unsigned long)(now - before) >= 60000) {
    check_battery();
    before = now;
    if (SD.exists("datalog.txt")) {
      Serial.println("File datalog.txt exists on card");
      snprintf(mqtt_message, 75, "%01d,%01d,%04d,%ld,%ld", node_id, button1_state, button2_state, analog_in, now, get_rtc());
      mqtt_pub("/node/5/adc", mqtt_message);
    }
    //write_sd();
  }
  if (button1_state != button1_state_last) {
    button1_state_last = button1_state;
    snprintf(mqtt_message, 40, "%01d", button1_state);
    mqtt_pub("/node/5/button/1", mqtt_message);
  }
  else if (button2_state != button2_state_last) {
    button2_state_last = button2_state;
    snprintf(mqtt_message, 40, "%01d", button2_state);
    mqtt_pub("/node/5/button/2", mqtt_message);
  }
*/
 

/*
  // let em data fly
  wifi_awake();
  Serial.println(client.state());
  if (client.connected ()){
     client.loop();
  }
  else {
    Serial.println("MQTT Broker Connection Down");
    if (client.connect("node6","shed/will",0,0,"Shed Temp Fail")) {
       client.publish("nodes","node6 reconnected");
       Serial.println("MQTT Broker Connection Restated");
    }
    else {
      // Don't flood the broker with connections and it's already down so we don't need to worry about a delay disconnecting the client
      delay(1000);
      Serial.println("MQTT Connection Delay");
    }
  }
  Serial.println(client.state());
  //setup_mqtt();
  //Serial.println(client.state());
  //mqtt_reconnect();
  //Serial.println(client.state());
  //setup_mqtt();
  //client.loop();
  Serial.println(client.state());
  snprintf(mqtt_message, 100, "node %02d - %01d,%01d,%04d,%ld", node_id, button1_state, button2_state, analog_in, now);
  mqtt_pub("nodes", mqtt_message);
  
  client.disconnect();
  Serial.println(client.state());
  wifi_sleep();
*/

/*
  //todo:  real deep sleep also does not work atm
  //ESP.deepSleep(sleepTime * 1000);
  //ESP.reset();
*/
