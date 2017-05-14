#ifndef __SYS_LOG_H__
#define __SYS_LOG_H__

#include <fstream>

class SysLog: public std::ofstream {
public:
  static class SysLog* Instance();

public:
  ~SysLog();

private:
  SysLog();

  void Open();

private:
  static class SysLog* m_instance;
};

#endif
