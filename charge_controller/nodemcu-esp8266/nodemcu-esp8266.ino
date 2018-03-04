/*
   Software for windpower charge controller on NodeMCU
     <https://systemausfall.org/wikis/howto/Eddy-2-Power>

   blinking onboard LED means:
     fast  - connecting to wireless network
     slow  - connecting to mqtt broker
     flash - sending bytes via mqtt
     on    - critical power supply

   mqtt channel scheme:
     /node/X/<sensor> - publish from node X the value(s) of corresponding
                        sensor (e.g. adc, mcp, internal vcc)
     /node/X/status   - node status in csv format (e.g. for munin)
     /node/X/json     - node status in json format (e.g. for influxdb, grafana)

   TODO: wifi sleep mode and deep sleep don't work atm
    ESP.deepSleep(sleep_time);
    ESP.reset();
*/
/*
  Copyright 2018 Henning Rieger <age@systemausfall.org>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  Dieses Programm ist Freie Software: Sie können es unter den Bedingungen
  der GNU General Public License, wie von der Free Software Foundation,
  Version 3 der Lizenz oder (nach Ihrer Wahl) jeder neueren
  veröffentlichten Version, weiterverbreiten und/oder modifizieren.

  Dieses Programm wird in der Hoffnung, dass es nützlich sein wird, aber
  OHNE JEDE GEWÄHRLEISTUNG, bereitgestellt; sogar ohne die implizite
  Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
  Siehe die GNU General Public License für weitere Details.

  Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
  Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_ADS1015.h>
#include <MCP3208.h>
#include <EEPROM.h>

/*
   Change the following values to fit your setup.
*/
/* wifi */
const char* ssid = "";
const char* password = "";

/* mqtt broker */
#define NODE_NAME "node1"
const String mqtt_channel = "node/1/";
const char* mqtt_server = "192.168.1.111";
const int mqtt_port = 1883;

/* sleep time between measurements in milliseconds */
const int sleep_time = 5000;
const int low_voltage = 1000;
int read_repeat = 10; // reread how often for measure interpolation

/* anemometer, rpmometer */
const byte interrupt_pin1 = 5;  // nodemcu pin: D1
const byte interrupt_pin2 = 4;  // nodemcu pin: D2

/* output pins e.g. leds and relais */
const int led1 = 16;            // nodemcu pin: D0 (and built-in LED)
const int switch1 = 0;            // nodemcu pin: D3
const int switch2 = 2;            // nodemcu pin: D4

/* sd card */
const int chip_select = 10;     // nodemcu pin: SD2

/* real time clock */
const int rtc_SDA = 10;          // nodemcu pin: SD2
const int rtc_SDC = 9;          // nodemcu pin: SD3

/* mcp3208 SPI connections */
#define CS_PIN 14               // nodemcu pin: D5 - mcp: CS
#define MOSI_PIN 12             // nodemcu pin: D6 - mcp: D_in
#define MISO_PIN 13             // nodemcu pin: D7 - mcp: D_out
#define CLOCK_PIN 15            // nodemcu pin: D8 - mcp: CLK

/*
   Don't change after here, unless you know what you do.
*/
int analog_in = 0;
unsigned long now = 0;
unsigned long before = 0;
unsigned long rtc_now;
unsigned long time_delta;
char mqtt_message[1000]; // increase in PubSubClient.h: MQTT_MAX_PACKET_SIZE 1024
char sd_data;
int adc0, adc1, adc2, adc3;
int mcp[8];
volatile int interruptCounter1 = 0;
volatile int interruptCounter2 = 0;
unsigned long wifi_connects = 0;
unsigned long mqtt_connects = 0;
unsigned long mqtt_message_counts = 0;
int switch1_state = 0;
int switch2_state = 0;

ADC_MODE(ADC_VCC); //read ESP voltage instead of analog_in A0, leave A0 unconnected
MCP3208 mcp3208(CLOCK_PIN, MOSI_PIN, MISO_PIN, CS_PIN);
IPAddress local_ip(127, 0, 0, 0); // will be set via dhcp
WiFiClient espClient;
PubSubClient client(espClient);
Sd2Card card;
SdVolume volume;
SdFile root;
RTC_PCF8523 rtc;
DateTime rtc_time;
Adafruit_ADS1015 ads;

/*
   bring everything up and running
*/
void setup() {
  pinMode(led1, OUTPUT);
  pinMode(switch1, OUTPUT);
  digitalWrite(led1, 1);
  Serial.begin(115200);
  setup_wifi();
  setup_mqtt();
  setup_interrupts();     // for anemometer
  //setup_adc();
  //setup_sd();
  //setup_rtc();
}

/*
   main loop
*/
void loop() {
  reset_interrupt_counter();
  check_time();
  check_mcp();
  check_internal_voltage();
  //check_battery_voltage();
  //check_temperature();

  delay(sleep_time);

  submit_interrupts();
  submit_status();
  submit_mcp();
  //submit_internal_voltage();

  //check_adc();
  //Serial.print("rtc: ");
  //Serial.println(get_rtc());
  /*
    read_eeprom();
    clear_eeprom();
    read_eeprom();
    write_eeprom();
    read_eeprom();
  */
}

/*
 * pwm tests
 */
void pwm() {
  int pwm = mcp[2] / 4;
  analogWrite(switch1, pwm);
  //Serial.print("pwm: ");
  //Serial.println(pwm);
}

/*
   submit internal state via mqtt
   fields are:
      time from start in ms, looptime in ms, wifi connection count,
      mqtt connection count, mqtt message count, switch1 status, switch2 status
      interrupt 1 counter, interrupt2 counter
*/
void submit_status() {
  snprintf(mqtt_message, 110, "%ld,%ld,%ld,%ld,%ld,%i,%i,%i,%i,%i", now, time_delta, wifi_connects, mqtt_connects, mqtt_message_counts, switch1_state, switch2_state, interruptCounter1, interruptCounter2, analog_in);
  mqtt_pub(mqtt_channel + "status", mqtt_message);
  // also send in json format for telegraf -> influxdb
  snprintf(mqtt_message, 1000, "{\"uptime\":%ld, \"looptime\":%ld, \"wifi\":%ld, \"mqtt1\":%ld, \"mqtt2\":%ld, \"s1\":%i, \"s2\":%i, \"i1\":%i, \"i2\":%i \, \"vcc\":%i }",
           now, time_delta, wifi_connects, mqtt_connects,  mqtt_message_counts, switch1_state, switch2_state, interruptCounter1, interruptCounter2, analog_in);
  mqtt_pub(mqtt_channel + "json", mqtt_message);
}

/*
   switch dumpload relais depending on measured voltage
*/
void check_battery_voltage() {
  //Serial.println(mcp[1]);
  if (mcp[2] > low_voltage and switch1_state < 1 ) {
    Serial.println("High voltage, switching dumpload on.");
    digitalWrite(switch1, 1);
    switch1_state = 1;
    snprintf(mqtt_message, 100, "%ld,%i", now, switch1_state);
    mqtt_pub(mqtt_channel + "switch1", mqtt_message);
  }
  else if (mcp[2] < low_voltage and switch1_state > 0 ) {
    Serial.println("Low voltage, switching dumpload off.");
    digitalWrite(switch1, 0);
    switch1_state = 0;
    snprintf(mqtt_message, 100, "%ld,%i", now, switch1_state);
    mqtt_pub(mqtt_channel + "switch1", mqtt_message);
  }
}

/*
   react on high temperatures
*/
void check_temperature() {
  //TODO
}

/*
   EEPROM Reading & Writing
*/
void begin_eeprom() {
  EEPROM.begin(100);
}

void read_eeprom() {
  begin_eeprom();
  int eeprom_address = 0;
  byte eeprom_value;

  for (int i = 1; i <= 3; i++) {
    eeprom_address = i;

    // read a byte from the current address of the EEPROM
    eeprom_value = EEPROM.read(eeprom_address);
    Serial.print("eeprom: ");
    Serial.print(eeprom_address);
    Serial.print("\t");
    Serial.print(eeprom_value, DEC);
    Serial.println();
  }
  end_eeprom();
}

void clear_eeprom() {
  begin_eeprom();
  for (int i = 1; i <= 3; i++) {
    EEPROM.write(i, 0);
  }
  end_eeprom();
}

void write_eeprom() {
  begin_eeprom();
  int addr = 1;
  int val = ESP.getVcc();
  Serial.print("Vcc as int: ");
  Serial.println(val);
  EEPROM.write(addr, lowByte(val));
  EEPROM.write(addr + 1, highByte(val));
  Serial.print("low byte: ");
  Serial.println(EEPROM.read(addr));
  Serial.print("high byte: ");
  Serial.println(EEPROM.read(addr + 1));

  int combined;
  combined = highByte(val);              //send x_high to rightmost 8 bits
  combined = combined << 8;       //shift x_high over to leftmost 8 bits
  combined |= lowByte(val);                 //logical OR keeps x_high intact in combined and fills in rightmost 8 bits
  Serial.print("recombined: ");
  Serial.println(combined);
  end_eeprom();
}

void end_eeprom() {
  //EEPROM.commit();  // write changes to flash
  EEPROM.end();     // commit and release RAM copy of EEPROM content
}


/*
   Anemometer; count interrupts in time delta
*/
void setup_interrupts() {
  pinMode(interrupt_pin1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interrupt_pin1), handle_interrupt1, FALLING);
  pinMode(interrupt_pin2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interrupt_pin2), handle_interrupt2, FALLING);
}

void reset_interrupt_counter() {
  interruptCounter1 = 0;
  interruptCounter2 = 0;
}

void handle_interrupt1() {
  //digitalWrite(led2, interruptCounter1 % 2);
  interruptCounter1++;
  //Serial.println(interruptCounter1 % 2);
  //Serial.println(interruptCounter1);
}

void handle_interrupt2() {
  interruptCounter2++;
  //Serial.println(interruptCounter2);
}

void submit_interrupts() {
  // interrupts since last loop
  //Serial.print("occured interrupts 1 in last loop: ");
  //Serial.println(interruptCounter1);
  //Serial.print("occured interrupts 2 in last loop: ");
  //Serial.println(interruptCounter2);
  snprintf(mqtt_message, 100, "%ld,%i,%i", time_delta, interruptCounter1, interruptCounter2);
  mqtt_pub(mqtt_channel + "interrupts", mqtt_message);
}

/*
   handle loop time
   keep in mind: millis() will start again at 0 about every 50 days
*/
void check_time() {
  before = now;
  now = millis();
  if (now < before) {
    before = now;
  }
  time_delta = now - before;

  /*
    Serial.print("uptime: ");
    Serial.println(now);
    Serial.print("time for last loop: ");
    Serial.println(time_delta);
    snprintf(mqtt_message, 100, "%ld,%ld,%ld,%ld,%ld", now, time_delta, wifi_connects, mqtt_connects, mqtt_message_counts);
    mqtt_pub(mqtt_channel + "uptime", mqtt_message);
  */
}

/*
   ADC
*/
void setup_adc() {
  /*
     The ADC input range (or gain) can be changed via the following
     functions, but be careful never to exceed VDD +0.3V max, or to
     exceed the upper and lower limits if you adjust the input range!
     Setting these values incorrectly may destroy your ADC!
                                                                    ADS1015  ADS1115
                                                                    -------  -------
     ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
     ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
     ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
     ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
     ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
     ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
  */
  ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV
  ads.begin();
  Serial.println("ADC initialized");
}

void check_adc() {
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
   MCP3208  analog/digital converter
*/
void check_mcp() {
  /*
     The MCP3208 has eight 12bit analog inputs.
     We are reading them here one after another.
     valid return values are between 0 - 4095
     returns 8191 if no mcp3208 is present
  */
  //Serial.print("MCP3208 values:");
  for (int i = 0; i <= 7; i++) {
    mcp[i] = 0;
    // read several times and interpolate
    for (int j = 0; j < read_repeat; j++) {
      mcp[i] += mcp3208.readADC(i);
      delay(10);
      //Serial.print(mcp[i]);
      //Serial.print(", ");
    }
    //Serial.print(mcp[i] % read_repeat);
    // calculate average, eventually round up and convert back to int
    mcp[i] = (int)(mcp[i] / read_repeat + 0.5);
    //Serial.print(" middle: ");
    //Serial.print(mcp[i]);
    //Serial.print(", modulo: ");
    //Serial.println(mcp[i] % read_repeat);
    /*
    if (mcp[i] > 4096) {
      Serial.print("MCP3208 channel: ");
      Serial.print(i);
      Serial.print(", value: ");
      Serial.print(mcp[i]);
      Serial.println(". Values above 4096 are indicating errors with MCP3208.");
    }
    */
  }
  //Serial.println("");
}

void submit_mcp() {
  snprintf(mqtt_message, 100, "%04i,%04i,%04i,%04i,%04i,%04i,%04i,%04i", mcp[0],  mcp[1], mcp[2], mcp[3], mcp[4], mcp[5], mcp[6], mcp[7]);
  mqtt_pub(mqtt_channel + "mcp3208", mqtt_message);
  snprintf(mqtt_message, 1000, "{\"0\":%i, \"1\":%i, \"2\":%i, \"3\":%i, \"4\":%i, \"5\":%i, \"6\":%i, \"7\":%i }",
           mcp[0],  mcp[1], mcp[2], mcp[3], mcp[4], mcp[5], mcp[6], mcp[7] );
  mqtt_pub(mqtt_channel + "mcp3208.json", mqtt_message);
}

/*
   real time clock
*/
void setup_rtc() {
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
   SD Card
*/
void setup_sd() {
  if (!SD.begin(chip_select)) {
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
    analog_in = 5;
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
   wifi
*/
void setup_wifi() {
  WiFi.softAPdisconnect(true);
  WiFi.disconnect(true);
  WiFi.persistent(false); //  every wifi.begin() writes to flash, this stops it
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Starting wifi connection");
  // Try max. 20 seconds to connect to access point.
  //while(WiFi.status() != WL_CONNECTED) {
  for (int i = 0; i <= 79; i++) {
    // blink slowly while connecting
    digitalWrite(led1, 0);
    delay(125);
    digitalWrite(led1, 1);
    delay(125);
    digitalWrite(led1, 0);
    Serial.print(".");
    if (WiFi.status() == WL_CONNECTED) {
      i = 80;
      wifi_connects += 1;
      mqtt_connects = 0;
      mqtt_message_counts = 0;
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
   putting wifi to sleep and waking up again does not work properly with mqtt
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
   MQTT
*/
void setup_mqtt() {
  client.setServer(mqtt_server, mqtt_port);
}
void mqtt_reconnect() {
  client.disconnect();
  mqtt_connect();
}

void mqtt_connect() {
  Serial.println("Connecting to MQTT broker.");
  // Try three times to reconnect to mqtt broker.
  //while (!client.connected()) {
  for (int i = 0; i <= 2; i++) {
    if (client.connect(NODE_NAME)) {
      Serial.println("Connection to MQTT broker is established.");
      client.publish("node", "hello.");
      mqtt_connects += 1;
      client.loop();
      i = 3;
    }
    else {
      // retry mqtt connection three times, otherwise move on
      Serial.print("Conenction to MQTT broker failed. Client state: ");
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
    Serial.println("MQTT broker seems to be not available, will wait some random seconds and restart full wifi stack.");
    delay(random(1, 20) * 1000);
    setup_wifi();
  }
}

void mqtt_check_connection() {

}

//void mqtt_pub(char* channel, char* mqtt_message) {
void mqtt_pub(String channel, char* mqtt_message) {
  //Serial.print("MQTT connection status: ");
  //Serial.println(client.connected());
  if (!client.connected()) {
    mqtt_connect();
  }
  else {
    char mqtt_channel[100];
    channel.toCharArray(mqtt_channel, 100);
    Serial.print("MQTT channel: ");
    Serial.println(mqtt_channel);
    Serial.print("MQTT message: ");
    Serial.println(mqtt_message);
    client.publish(mqtt_channel, mqtt_message);
    digitalWrite(led1, 0);
    delay(1);
    digitalWrite(led1, 1);
    mqtt_message_counts += 1;
  }
}

/*
   batteries; internal power supply
*/
void check_internal_voltage() {
   analog_in = 0;
   for (int j = 0; j < read_repeat; j++) {
      analog_in += ESP.getVcc();
      delay(5);
    }
    // calculate average and eventually round up
    if ((analog_in % read_repeat) >= 5) {
      analog_in /= read_repeat;
      analog_in += 1;
    }
    else {
      analog_in /= read_repeat;
    }
}

void submit_internal_voltage() {
  //analog_in = ESP.getVcc();
  //Serial.print("VCC value: ");
  //Serial.println(analog_in);
  snprintf(mqtt_message, 100, "%04i", analog_in);
  mqtt_pub(mqtt_channel + "vcc", mqtt_message);
}
