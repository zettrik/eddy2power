/*
  Example for MCP3008 - Library for communicating with MCP3008 Analog to digital converter.
  Created by Uros Petrevski, Nodesign.net 2013
  Released into the public domain.
*/

#include <MCP3208.h>


// define pin connections
#define CS_PIN 14
#define CLOCK_PIN 15
#define MOSI_PIN 12
#define MISO_PIN 13

ADC_MODE(ADC_VCC); // read ESP voltage instead of analog_in A0, leave A0 unconnected
MCP3208 adc(CLOCK_PIN, MOSI_PIN, MISO_PIN, CS_PIN); // put pins inside MCP3008 constructor

void setup() {
 
 // open serial port
 Serial.begin(115200);
  
}


void loop() {
  
  int val = ESP.getVcc(); // analogRead(A0);
  Serial.print("internal voltage: ");
  Serial.println(val/1000);

  // read all MCP3208 channels
  Serial.println("MCP3208 values:");
  for (int i=0; i<=7; i++) {
    val = adc.readADC(i);
    Serial.print(val);
    Serial.print("\t");
  }
  Serial.println("");

  delay(3000);  
}
