#ifndef __STRING_UTILS_H__
#define __STRING_UTILS_H__

#include <string>

using std::string;

string lstrip(const string str);
string rstrip(const string str);
string strip(const string str);

bool IsNum(const string str);

#endif
