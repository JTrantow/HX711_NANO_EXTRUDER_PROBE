/*!
    \file Running_Statistics.h

  https://www.johndcook.com/blog/standard_deviation/
  This better way of computing variance goes back to a 1962 paper by B. P. Welford and is presented in Donald Knuth’s Art of Computer Programming, Vol 2, page 232, 3rd edition.

  MODIFIED to use floats instead of doubles???
*/
#pragma once

#include "Arduino.h"

/*!
    Original code uses doubles for statistic calculations.
    Using floats may reduce calculations(and precision) for some processors.

    Precision options:
        - float single precision floating-point type. Usually IEEE-754 binary32 format.
        - double double precision floating-point type. Usually IEEE-754 binary64 format.
        - long double extended precision floating-point type. Does not necessarily map to types mandated by IEEE-754 and does NOT offer additional precision with most Arduino boards. 

    The Arduino nano implements float, double, long double as a binary32. 
    There is no difference between the three fp type on a nano.

*/
typedef double rs_precision_t;  //!< Define floating point precision for running statistics class.
typedef uint32_t rs_uint_t;     //!< Define uint size for running statistics class. This determines number of samples before rollover.

/*!
    \brief Library for the HX711 load cell amplifier.
*/
class RunningStat    
{
    public:
        RunningStat();
        void Clear();
        void Push(rs_precision_t x);
        rs_uint_t NumDataValues() const;
        rs_precision_t Mean() const;
        rs_precision_t Variance() const;
        rs_precision_t StandardDeviation() const;
        
    private:
        rs_uint_t m_n;
        rs_precision_t m_oldM, m_newM, m_oldS, m_newS;
};