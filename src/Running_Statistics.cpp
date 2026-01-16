/*!
    \file Running_Statistics.cpp

  https://www.johndcook.com/blog/standard_deviation/
  This better way of computing variance goes back to a 1962 paper by B. P. Welford and is presented in Donald Knuth’s Art of Computer Programming, Vol 2, page 232, 3rd edition.

  MODIFIED to use floats instead of doubles???
*/
#include <Arduino.h>
#include "Running_Statistics.h"

RunningStat::RunningStat() : m_n(0) {}

void RunningStat::Clear()
{
    m_n = 0;
}

void RunningStat::Push(rs_precision_t x)
{
    m_n++;

    // See Knuth TAOCP vol 2, 3rd edition, page 232
    if (m_n == 1)
    {
        m_oldM = m_newM = x;
        m_oldS = 0.0;
    }
    else
    {
        m_newM = m_oldM + (x - m_oldM)/m_n;
        m_newS = m_oldS + (x - m_oldM)*(x - m_newM);

        // set up for next iteration
        m_oldM = m_newM; 
        m_oldS = m_newS;
    }
}

rs_uint_t RunningStat::NumDataValues() const
{
    return m_n;
}

rs_precision_t RunningStat::Mean() const
{
    return (m_n > 0) ? m_newM : 0.0;
}

rs_precision_t RunningStat::Variance() const
{
    return ( (m_n > 1) ? m_newS/(m_n - 1) : 0.0 );
}

rs_precision_t RunningStat::StandardDeviation() const
{
    return sqrt( Variance() );
}
