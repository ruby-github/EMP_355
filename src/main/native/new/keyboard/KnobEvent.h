#ifndef __KNOB_EVENT_H__
#define __KNOB_EVENT_H__

#include "AbsKeyboardEvent.h"

class CKnobEvent: public AbsKeyboardEvent {
public:
  static void FEvent(unsigned char keyValue);

public:
  virtual ~CKnobEvent() {};

  virtual void KnobEvent(unsigned char keyValue, unsigned char offset);
};

#endif
