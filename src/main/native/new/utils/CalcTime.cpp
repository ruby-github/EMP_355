#include "utils/CalcTime.h"

#include <cstddef>

CalcTime::CalcTime() {
  Init();
}

CalcTime::~CalcTime() {
}

void CalcTime::Begin() {
  gettimeofday(&m_start, NULL);
}

void CalcTime::Restart() {
  Init();
  Begin();
}

double CalcTime::End() {
  gettimeofday(&m_end, NULL);

  double duration = (m_end.tv_sec - m_start.tv_sec) * 1000000 + (m_end.tv_usec - m_start.tv_usec);
  m_start = m_end;

  return duration;
}

//----------------------------------------------------------

void CalcTime::Init() {
  m_start.tv_sec = 0;
  m_start.tv_usec = 0;

  m_end.tv_sec = 0;
  m_end.tv_usec = 0;
}
