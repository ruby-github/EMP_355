#ifndef __KNOB_MENU_H__
#define __KNOB_MENU_H__

#include "utils/Utils.h"

#include "Def.h"
#include "periDevice/ManRegister.h"

class KnobMenu {
public:
  static KnobMenu* GetInstance();

public:
  ~KnobMenu();

  enum EKnobType {
    NONE, D2, M, PW, CFM, REPLAY, SNAP, ARROW, BDMK, ANATOMIC_M, EFOV
  };

  typedef EKnobReturn (*pKnobFunc)(EKnobOper);
  typedef EKnobReturn (*pKnobFuncPress)();

  struct KnobItem {
    string name;
    string value;
    EKnobReturn status;
    pKnobFunc pf;
    pKnobFuncPress pfPress;
  };

  GtkWidget* Create();

  void SetItem(KnobItem* item, unsigned int total_item, EKnobType type);
  void SetValue(int index, const string value, EKnobReturn status);
  void Update();

  void PageUp();
  void PageDown();

  void Knob1_Press();
  void Knob2_Press();
  void Knob3_Press();
  void Knob4_Press();
  void Knob5_Press();
  void Knob6_Press();

  void Knob1_Screw(int cw);
  void Knob2_Screw(int cw);
  void Knob3_Screw(int cw);
  void Knob4_Screw(int cw);
  void Knob5_Screw(int cw);
  void Knob6_Screw(int cw);

private:
  KnobMenu();

  string Display(KnobItem item, const string tmp);
  void Refresh();

private:
  static KnobMenu* m_instance;
  static const int KNOB_NUM = 6;

private:
  GtkButton* m_button_left;
  GtkButton* m_button_right;
  GtkButton* m_button_Knob[KNOB_NUM];

  KnobItem* m_KnobItem;

  unsigned int m_CurLevel;
  unsigned int m_MaxLevel;
  EKnobType m_knobType;
};

#endif
