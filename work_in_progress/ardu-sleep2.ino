#include <avr/power.h>
#include <avr/sleep.h>

int u = 0;
int r = 3;

void sleepNow()
{  
     
  digitalWrite(r, LOW);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here

  sleep_enable();          // enables the sleep bit in the mcucr register
                          // so sleep is possible. just a safety pin 
  power_adc_disable();
  power_spi_disable();
  power_timer0_disable();
  power_timer1_disable();
  power_timer2_disable();
  power_twi_disable();
  
  sleep_mode();            // here the device is actually put to sleep!!   
}

void setup() {
  // put your setup code here, to run once:
  pinMode(r, OUTPUT);
  digitalWrite(r, LOW);
  delay(1000);
  u = analogRead(A1);
  delay(1000);
  u = analogRead(A1);
  if (u > 750) digitalWrite(r, HIGH); else sleepNow();
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(30000);  
  u = analogRead(A1);
  if (u < 750) sleepNow();
}
