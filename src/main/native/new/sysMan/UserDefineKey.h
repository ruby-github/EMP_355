#ifndef __USER_DEFINE_KEY_H__
#define __USER_DEFINE_KEY_H__

class UserDefineKey {
public:
  UserDefineKey();
  ~UserDefineKey();

public:
  void PressKeyP1();
  void PressKeyP2();
  void PressKeyP3();

private:
  enum EKeyFunc {NONE, TSI, SAVE_CINE, PRINT, STEER, PIP, PW_SOUND, HPRF, IMAGE_PRESET, BIOPSY};

  void FactoryCreate(EKeyFunc type);
};

#endif
