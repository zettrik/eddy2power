extern "C" {
  #include "user_interface.h"
}

void setup() {
  Serial.begin(115200);
  Serial.println();
}

void loop() {
  Serial.println("Go to light sleep, 10 secs");
  wifi_station_disconnect();
  wifi_set_opmode(NULL_MODE); 
  wifi_fpm_set_sleep_type(LIGHT_SLEEP_T); // LIGHT or MODEM doesn't seem to make a difference
  wifi_fpm_open();
  Serial.print("wifi_fpm_get_sleep_type: "); Serial.println(wifi_fpm_get_sleep_type());

  int resp = wifi_fpm_do_sleep(10000000); // 10 secs
  Serial.print("wifi_fpm_do_sleep resp: "); Serial.println(resp);
  delay(20000);  // 20 secs, so should light sleep for just the first 10 secs then go to full power

  Serial.println("delay 10 secs..."); // Now delay using full power
  delay(10000);
}
