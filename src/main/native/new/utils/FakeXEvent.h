#ifndef __FAKE_X_EVENT_H__
#define __FAKE_X_EVENT_H__

#include "AbsKeyboardEvent.h"

class FakeXEvent: public AbsKeyboardEvent {
public:
  virtual ~FakeXEvent();

public:
  virtual void KeyEvent(unsigned char keyvalue);
  virtual void MouseEvent(char offsetx, char offsety);
};

#endif
