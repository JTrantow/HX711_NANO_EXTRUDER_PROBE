/*!
  \file main.cpp
  
*/
#include "Adafruit_HX711.h"
#include "Running_Statistics.h"
#include "TimingTest.h"

// Define the pins for the HX711 communication connection (to Nano)
const uint8_t HX711_SCK_PIN  = 2;  //!< Output pin connected to HX711, initialized by Adafruit_HX711 class.
const uint8_t HX711_DATA_PIN = 3; //!< Input pin connected to HX711, initialized by Adafruit_HX711 class.
const uint8_t HX711_RATE_PIN = 4; //!< Low results in 10 SPS, High 80 SPS.

const uint8_t PROBE_TRIGGERED_PIN = LED_BUILTIN; //!< Output pin shared with onboard LED (D13 for Nano) which indicates triggered state.
const uint8_t PROBE_TARE_PIN = 14; //!< Input pin in case you only want to trigger when enabled. (not implemented)

Adafruit_HX711 hx711(HX711_DATA_PIN, HX711_SCK_PIN);

#define MINIMUM_SAMPLES (10)    //!< Number of initial samples before testing samples against STD.
#define NUM_STD (6) //!< Number of standard deviations used in the comparison window.

#define ABSOLUTE_FORCE (1UL<<20) //!< Samples with abs() > ABSOLUTE_FORCE with trigger.

RunningStat rs; //!< Class for computing running statistics (Mean and Standard Deviation)

/*!
  \brief setup

  Initialize Serial, pins, hx711 class and the RunningStat class.
*/
void setup(){

  Serial.begin(115200);
  Serial.println("Adafruit HX711 Based Extruder Nozzle Probe.");

  /*
    Initialize pin Modes and initial states.
  */
  pinMode(HX711_RATE_PIN, OUTPUT);
  pinMode(PROBE_TRIGGERED_PIN, OUTPUT);
  pinMode(PROBE_TARE_PIN, INPUT);

  #if (0)
    digitalWrite(HX711_RATE_PIN, LOW);   // Start at 10 SPS rate.
  #else
    digitalWrite(HX711_RATE_PIN, HIGH); // 80 SPS.
  #endif
  digitalWrite(PROBE_TRIGGERED_PIN, LOW);   

  /*
    Initialize the HX711 class.
  */
  hx711.begin();

  #if (1)
  // read and toss 3 values each
  Serial.println("Tareing....");

  for (uint8_t t=0; t<3; t++) {
    hx711.tareA(hx711.readChannelRaw(CHAN_A_GAIN_128));
    hx711.tareA(hx711.readChannelRaw(CHAN_A_GAIN_128));
    hx711.tareB(hx711.readChannelRaw(CHAN_B_GAIN_32));
    hx711.tareB(hx711.readChannelRaw(CHAN_B_GAIN_32));
  }
  #endif

  #if defined(TIMING_TEST_STATISTICS)
    TimingTest(10000);
  #endif
  /*
    Initialize the Running Statistics class.
  */
  Serial.println("Gather initial stats....");

  // Toss initial few values. Not sure this is necessary (beyond initial read which sets gain.)
  for (unsigned int t=0; t < 100; t++) {    
    (void)hx711.readChannelRaw(CHAN_A_GAIN_128);
  }

  for (unsigned int t=0; t < MINIMUM_SAMPLES; t++) {    
    rs.Push(hx711.readChannelRaw(CHAN_A_GAIN_128));
  }

  Serial.print("Initialized N ");
  Serial.print(rs.NumDataValues());
  Serial.print(" \tMean ");
  Serial.print(rs.Mean());
  Serial.print(" \tSTD ");
  Serial.println(rs.StandardDeviation());
} 


/*!
  \brief Main loop. Reads samples and compares statistics to thresholds.
*/
void loop() {

  int32_t sample;
  static bool probe_enabled = false;
  /*
    Read a new sample.
    Don't put triggered outliers into running statistics.
  */
  sample = hx711.readChannelRaw(CHAN_A_GAIN_128);

  /*
    Really crude check.
  */
  if (abs(sample) > ABSOLUTE_FORCE) {
    digitalWrite(PROBE_TRIGGERED_PIN, HIGH); 

    Serial.print("TRIGGERED ABS(sample) Channel A (Gain 128): ");
    Serial.println(sample);
    Serial.println(ABSOLUTE_FORCE);

  }else
  {
    rs_precision_t unbiased_sample = sample - rs.Mean();
    if (fabs(unbiased_sample) > NUM_STD * rs.StandardDeviation()) {
      digitalWrite(PROBE_TRIGGERED_PIN, HIGH);

      Serial.print("TRIGGERED ");
      Serial.print(sample);

      Serial.print(" \t|unbiased_sample| ");
      Serial.print(fabs(unbiased_sample));

      Serial.print(" \tN ");
      Serial.print(rs.NumDataValues());
    
      Serial.print(" \tMean ");
      Serial.print(rs.Mean());

      Serial.print(" \tSTD ");
      Serial.println(rs.StandardDeviation());



      rs.Push(sample); // Should I push when triggered????



    }else{
    // Sample is within STDs.
      digitalWrite(PROBE_TRIGGERED_PIN, LOW);
      rs.Push(sample);
  
      if (0 == (rs.NumDataValues() % (80*5)) ) {
        // Occasionally report statistics.
        Serial.print(" \tN ");
        Serial.print(rs.NumDataValues());
      
        Serial.print(" \tMean ");
        Serial.print(rs.Mean());

        Serial.print(" \tSTD ");
        Serial.println(rs.StandardDeviation());
      }
    }

  }


  /*
    We can use probe enable signal to reset statistics, change rate, or power_on either when probe starts or ends???
  */
  if (LOW != digitalRead(PROBE_TARE_PIN))
  {    
    if (!probe_enabled)
    {
//      rs.Clear(); // Reset statistics at beginning of probe deploy
    }
    probe_enabled = true;
    Serial.println("Probe enable.");
  }else if (probe_enabled)
  {
    probe_enabled = false;
  }
  


}
