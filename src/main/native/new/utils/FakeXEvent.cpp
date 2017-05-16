#include "utils/FakeXEvent.h"
#include "utils/FakeXUtils.h"

#include "keyboard/KeyDef.h"
#include "keyboard/KeyFunc.h"

FakeXEvent::~FakeXEvent() {
}

void FakeXEvent::KeyEvent(unsigned char keyvalue) {
  if(keyvalue == KEY_SHIFT_CTRL) {
    KeySwitchIM ksim;
    ksim.ExcuteChange(TRUE);

    return;
  }

  if(FakeMouseButton(keyvalue)) {
    return;
  }

  if(FakeNumKey(keyvalue)) {
    return;
  }

  if(FakeAlphabet(keyvalue)) {
    return;
  }

  if(FakePunctuation(keyvalue)) {
    return;
  }
}

void FakeXEvent::MouseEvent(char offsetx, char offsety) {
  FakeXMotionEventFullRange(offsetx, offsety);
}
