#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

enum EKnobOper {
  SUB, ADD, ROTATE
};

enum EKnobReturn {
  ERROR, OK, MIN, MAX, PRESS
};

enum EDrawMode {
  COPY, XOR
};

enum EStudy {
  ABD, OB, GYN, UR, CAR, SP, VES, ORT, EXTRA
};

typedef struct tagPOINT {
  int x;
  int y;
} POINT, *PPOINT;

#endif
