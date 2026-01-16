/*!
    \file TimingTest.cpp
*/
//#include "Arduino.h"
#include "Running_Statistics.h"
#include "TimingTest.h"

#if defined(TIMING_TEST_STATISTICS)

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
float TimingTest(class RunningStat rs, const unsigned int number_of_timing_samples)
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
