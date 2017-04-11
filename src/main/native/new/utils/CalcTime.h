#ifndef __calc_time_h__
#define __calc_time_h__

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
