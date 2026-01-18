/*!
    \file TimingTest.cpp
*/
#pragma once

#undef TIMING_TEST_STATISTICS  //!< #define or #undef to enable timing functionality (to decide on rs_precision_t)

#if defined(TIMING_TEST_STATISTICS)
  float TimingTest(class RunningStat rs, const unsigned int n);
#endif