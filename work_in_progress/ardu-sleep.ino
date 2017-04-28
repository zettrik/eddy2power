#include "LowPower.h"

void sleepForTwoMinutes()
{
  for(int i=0;i<15;i++)
      LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
}

