#ifndef __CALC_TIME_H__
#define __CALC_TIME_H__

#include <sys/time.h>

class CalcTime {
public:
  CalcTime();
  ~CalcTime();

  void Begin();
  void Restart();
  double End();

private:
  void Init();

private:
  struct timeval m_start;
  struct timeval m_end;
};

#endif
