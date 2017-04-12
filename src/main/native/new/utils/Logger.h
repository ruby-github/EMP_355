#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <string>

using std::string;

class Logger {
public:
  static void error(const string str);
  static void warning(const string str);
  static void info(const string str);
};

#endif
