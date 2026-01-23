/*!
  \file probe_enable.cpp
  
*/
#include "Probe_enable.h"
#include "Arduino.h"

#include "Running_Statistics.h"

extern RunningStat rs;  /* Running statistics class. */

/*!
  \brief Control the HX711 and/or statistics when probe enable/tare signal is received.

    We can use probe enable signal to reset statistics, change rate, or power_on/power_off when probing starts or ends.
    Run probe repeatability to choose best strategy.

    \param [in] pin_state use state of TARE or PROBE_ENABLE_DISABLE_PIN.
    \returns true when probe has been started and pin_state is true.
*/
bool probe_enable(bool pin_state)
{
  static bool probe_enabled = false;  
  static unsigned long tic;
  bool probe_started = false;

  if (LOW != pin_state)
  {  
    if (!probe_enabled)
    { /*
        Start of probing.
        There are several options to explore here.
        
        1) Let statistics run continuously.
        2) Reset statistics based probe enable signal. Marlin can be setup with PROBE_ENABLE_DISABLE or PROBE_TARE.
          #define PROBE_ENABLE_DISABLE
          #define PROBE_TARE
        3) Some other strategy???  
      */
      tic = millis();
      #if (0)
        /*
          If you let statistics run continuously it will average over all printing activity.
          This result is NOT the best metric for probling, but may be useful for crash detection.
        */
        Serial.println("Probe start. No special action, use running statistics.");
      #elif (1)
        /*
          Resetting the statistics at the start of probing allows you to only include data from this probe.
          For fast probing speeds, this may require a little extra time to get averages. (80 SPS is probably your friend)
          If you decide to power_up() power_down on, notice the power up delay is much shorter at 80 SPS.

          Use PROBE_ENABLE_DISABLE or PROBE_TARE.
          PROBE_ENABLE_DISABLE can be used to power_up/power_down.
          PROBE_TARE has some delay options but signal goes away when delays are done. (can't be used to power_down()).
        */
        rs.Clear(); // Reset statistics at beginning of probe deploy
        Serial.println("Probing start. Clear statistics.");
      #else
        Serial.println("New strategy???");
      #endif
    }
    probe_enabled = true;
  }else if (probe_enabled)
  {
    unsigned long toc = millis();

    Serial.print("Probing done ");
    Serial.print(toc-tic);
    Serial.println("[msec].");
    probe_enabled = false;
  }
  return(probe_enabled);
}