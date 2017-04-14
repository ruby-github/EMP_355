#include "utils/FakeXEvent.h"
#include "utils/FakeXUtils.h"

#include "keyboard/KeyDef.h"
#include "keyboard/KeyFunc.h"

FakeXEvent::~FakeXEvent() {
}

void FakeXEvent::KeyEvent(unsigned char keyvalue) {
  #if defined(EMP_322)
    if(keyvalue == KEY_CTRL_SHIFT_SPACE) {
      KeySwitchIM ksim;
      ksim.ExcuteChange(TRUE);

      return;
    }
  #elif defined(EMP_313)
    if (keyvalue == KEY_ONE) {
      KeySwitchIM ksim;
      ksim.ExcuteChange(TRUE);

      return;
    }
  #else
    if(keyvalue == KEY_SHIFT_CTRL) {
      KeySwitchIM ksim;
      ksim.ExcuteChange(TRUE);

      return;
    }
  #endif

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
