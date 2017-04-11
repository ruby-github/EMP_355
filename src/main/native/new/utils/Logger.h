#ifndef __logger_h__
#define __logger_h__

#include <string>

using std::string;

class Logger {
public:
  static void error(const string str);
  static void warning(const string str);
  static void info(const string str);
};

#endif
