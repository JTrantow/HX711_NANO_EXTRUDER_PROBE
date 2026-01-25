/*!
  \file main.cpp
  
*/
#include "Adafruit_HX711.h"
#include "Running_Statistics.h"
#include "TimingTest.h"
#include "Probe_enable.h"

// Define the pins for the HX711 communication connection (to Nano)
const uint8_t HX711_SCK_PIN  = 3;  //!< Output pin connected to HX711, initialized by Adafruit_HX711 class.
const uint8_t HX711_DATA_PIN = 2; //!< Input pin connected to HX711, initialized by Adafruit_HX711 class.
const uint8_t PROBE_TRIGGERED_PIN = LED_BUILTIN; //!< Output pin that indicates when probe has touched the bed. Use LED pin for visual.
const uint8_t PROBE_TARE_PIN = 21; //!< Input pin in case you only want to trigger when enabled. 

Adafruit_HX711 hx711(HX711_DATA_PIN, HX711_SCK_PIN); //!< Using the Adafruit library class.

#define SPS (80) //!< Samples per second. HX711 supports 10 or 80 if rate pin is available on the board.
#define MINIMUM_SAMPLES (SPS/2)    //!< Minimum number of initial samples before testing samples against STD.
#define NUM_STD (6) //!< Number of standard deviations used in the comparison window.

#define ABSOLUTE_FORCE (1UL<<20) //!< Samples with abs() > ABSOLUTE_FORCE with trigger.

RunningStat rs; //!< Class for computing running statistics (Mean and Standard Deviation)

/*!
  \brief Display strain statistics.
*/
void display_statistics(void) 
{
  Serial.print(" \tN ");
  Serial.print(rs.NumDataValues());

  Serial.print(" \tMean ");
  Serial.print(rs.Mean());

  Serial.print(" \tSTD ");
  Serial.println(rs.StandardDeviation());
}

/*!
  \brief setup

  Initialize Serial, pins, hx711 class and the RunningStat class.
*/
void setup(){

  Serial.begin(115200);
  Serial.println("Nano/HX711 Based Extruder Nozzle Probe.");

  /*
    Initialize pin Modes and initial states.
  */
  pinMode(PROBE_TRIGGERED_PIN, OUTPUT);
  pinMode(PROBE_TARE_PIN, INPUT);

  digitalWrite(PROBE_TRIGGERED_PIN, LOW);   
  /*
    Initialize the HX711 class.
  */
  hx711.begin();

  #if defined(TIMING_TEST_STATISTICS)
    TimingTest(10000);
  #endif

  // Toss initial few values. Not sure this is necessary (beyond initial read which sets gain.)
  for (unsigned int t=0; t < 3; t++) {    
    (void)hx711.readChannelRaw(CHAN_A_GAIN_128);
  }
  display_statistics();
} 

/*!
  \brief Main loop. Reads samples and compares statistics to thresholds.
*/
void loop() {
  static bool probe_started = false;
  int32_t sample;

  /*
    Read a new sample.
    Don't put triggered outliers into running statistics.
  */
  sample = hx711.readChannelRaw(CHAN_A_GAIN_128);
  probe_started = probe_enable(digitalRead(PROBE_TARE_PIN)) || probe_started;  // With PROBE_TARE the enable pin goes false before trigger.

  if (probe_started) {
    /*
      Probing has been started and will end when trigger occurs.
    */
    if (MINIMUM_SAMPLES <= rs.NumDataValues()) {
      /*
        Enough samples to compare.
      */
      static unsigned int triggered_count = 0;

      rs_precision_t unbiased_sample = sample - rs.Mean();

      if (fabs(unbiased_sample) > NUM_STD * rs.StandardDeviation()) {
        /*
          Trigger event.

          Only trigger for specific amount of time (500msec or less).
          This helps if the load cell settles to a new average after the probe.
          Use this with Clear on probe enable signal.
        */
        triggered_count++;
        if ((SPS/2) <= triggered_count) {
          triggered_count=0;
          digitalWrite(PROBE_TRIGGERED_PIN, LOW); // End the triggered event. 
          probe_started = false;                  

          rs.Clear();
          Serial.print("TRIGGER Stuck ");
        }else{
          digitalWrite(PROBE_TRIGGERED_PIN, HIGH);
          Serial.print("TRIGGERED ");
        }
        Serial.print(sample);

        Serial.print(" \t|unbiased_sample| ");
        Serial.print(fabs(unbiased_sample));

        display_statistics();        

  //      rs.Push(sample); // Don't push triggered values into statistics.
      }else{
        /*
          Sample is within specified STDs tolerance.
          Waiting for trigger event.
        */
        digitalWrite(PROBE_TRIGGERED_PIN, LOW);
        triggered_count=0;
        rs.Push(sample);
    
        /*
          Check for not reaching trigger within 5 seconds of probe start.
        */
        if ((10*SPS) < rs.NumDataValues()) 
        {
          Serial.println("No trigger signal level within 10 seconds. Abort.");
          probe_started = false;
        }
        
          // Occasionally(5 seconds) report statistics.
        if (0 == (rs.NumDataValues() % (10*5)) ) {
          display_statistics();
        }
      }
    }else{      
        // Not enough samples to compare.
      rs.Push(sample);
    }
  }else{
    // Probing has not been started. 
    /*
      Really crude check.
    */  
    if (abs(sample) > ABSOLUTE_FORCE) {
      digitalWrite(PROBE_TRIGGERED_PIN, HIGH); 

      Serial.print("TRIGGERED ABS(sample) Channel A (Gain 128): ");
      Serial.println(sample);
      Serial.println(ABSOLUTE_FORCE);
    }else{
      digitalWrite(PROBE_TRIGGERED_PIN, LOW); 
    }
    rs.Push(sample);
    // Occasionally(5 seconds) report statistics.
    if (0 == (rs.NumDataValues() % (SPS*5)) ) {
      display_statistics();
    }
  }
}
