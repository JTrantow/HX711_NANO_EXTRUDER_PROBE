/*!
  \file main.cpp
  
*/
#include "Adafruit_HX711.h"
#include "Running_Statistics.h"

// Define the pins for the HX711 communication connection (to Nano)
const uint8_t HX711_SCK_PIN  = 2;  //!< Output pin connected to HX711, initialized by Adafruit_HX711 class.
const uint8_t HX711_DATA_PIN = 3; //!< Input pin connected to HX711, initialized by Adafruit_HX711 class.
const uint8_t HX711_RATE_PIN = 4; //!< Low results in 10 SPS, High 80 SPS.

const uint8_t PROBE_TRIGGERED_PIN = LED_BUILTIN; //!< Output pin shared with onboard LED (D13 for Nano) which indicates triggered state.
const uint8_t PROBE_ENABLE_PIN = 6; //!< Input pin in case you only want to trigger when enabled. (not implemented)

Adafruit_HX711 hx711(HX711_DATA_PIN, HX711_SCK_PIN);

#define MINIMUM_SAMPLES (10)    //!< Number of initial samples before testing samples against STD.
#define NUM_STD (6) //!< Number of standard deviations used in the comparison window.

#define ABSOLUTE_FORCE (100000) //!< Samples with abs() > ABSOLUTE_FORCE with trigger.

RunningStat rs; //!< Class for computing running statistics (Mean and Standard Deviation)

#define TIMING_TEST_STATISTICS  //!< #define or #undef to enable timing functionality (to decide on rs_precision_t)

#if defined(TIMING_TEST_STATISTICS)
  rs_precision_t TimingTest(const unsigned int n);

/*!
    \brief Returns the number of usec to update and check the statistics class.

    Use this function to measure statistic calculations on your processor.

    Measures just the push, mean, std functions.

    Results on a 5V 16Mhz ATnega328P Nano board with 10000 samples:
      TimingTest( N= 10000) Time per sample 77.86 [usec].

    The HX711 runs at 10 or 80 SPS. 
    At 80 SPS we process a new sample every 12.5msec/12500usec.
    The statistics calculation time is negligible (0.6%) relative to HX711 rate.

    \param [in] number_of_timing_samples for this test.
    \returns usec per sample.
*/
rs_precision_t TimingTest(const unsigned int number_of_timing_samples)
{
    unsigned long tic = micros();
    unsigned long toc;

    Serial.print("TimingTest( N= ");
    Serial.print( number_of_timing_samples);

    tic = micros();

    for (unsigned int  t=0; t <  number_of_timing_samples; t++) {    
      volatile int32_t sample = 99;
      rs_precision_t new_sample;

      new_sample = sample;
      rs.Push(new_sample);
      if (fabs(sample-rs.Mean()) > (6 * rs.StandardDeviation()) ) 
      {
        Serial.print("Didn't expect this!");
      }
    }
    toc = micros();

    Serial.print(") Time per sample ");
    Serial.print((float)(toc-tic)/number_of_timing_samples);
    Serial.println(" [usec].");

    rs.Clear();

    return((float)(toc-tic)/number_of_timing_samples);
}
#endif
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
  pinMode(PROBE_ENABLE_PIN, INPUT);

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

  for (unsigned int t=0; t < MINIMUM_SAMPLES; t++) {    
    rs.Push(hx711.readChannelBlocking(CHAN_A_GAIN_128));
  }
  Serial.print("Initialized N ");
  Serial.print(rs.NumDataValues());
  Serial.print(" \tMean ");
  Serial.print(rs.Mean());
  Serial.print(" \tSTD ");
  Serial.println(rs.StandardDeviation());
} 


void loop() {

  int32_t sample = hx711.readChannelBlocking(CHAN_A_GAIN_128);

  /*
    Really crude check.
  */
  if (abs(sample) > ABSOLUTE_FORCE) {
    Serial.print("TRIGGERED ABS(sample) Channel A (Gain 128): ");
    Serial.println(sample);
    digitalWrite(PROBE_TRIGGERED_PIN, HIGH); // 80 SPS.
  }

  /*
    Check new samples after correcting for bias and variance.
  */
  rs_precision_t unbiased_sample = sample - rs.Mean();
  if (fabs(unbiased_sample) > 6.0 * rs.StandardDeviation()) {
    Serial.print("TRIGGERED on ");
    Serial.print(sample);

    Serial.print(" \tN ");
    Serial.print(rs.NumDataValues());
  
    Serial.print(" \tMean ");
    Serial.print(rs.Mean());

    Serial.print(" \tSTD ");
    Serial.println(rs.StandardDeviation());
  }else{
    // Don't put trigger outliers into running statistics.
    rs.Push(sample);
  }
}
