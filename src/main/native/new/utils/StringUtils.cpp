#include "utils/StringUtils.h"

#include <sstream>

using namespace std;

string lstrip(const string str) {
  size_t pos = 0;

  for (size_t i = 0; i < str.size(); i++) {
    if (!isspace(str[i])) {
      break;
    }

    pos = i + 1;
  }

  if (pos <= str.size() ) {
    return str.substr(pos);
  } else {
    return str;
  }
}

string rstrip(const string str) {
  size_t pos = str.size();

  for (size_t i = str.size(); i > 0; i--) {
    if (!isspace(str[i - 1])) {
      break;
    }

    pos = i - 1;
  }

  return str.substr(0, pos);
}

string strip(const string str) {
  return lstrip(rstrip(str));
}

bool IsNum(const string str) {
  istringstream iss(str);
  float i;
  iss >> i;

  return iss.rdstate() == std::ios_base::eofbit;
}