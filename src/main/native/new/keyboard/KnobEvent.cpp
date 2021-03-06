#include "keyboard/KnobEvent.h"

#include "display/KnobMenu.h"
#include "keyboard/KeyDef.h"

void CKnobEvent::FEvent(unsigned char keyValue) {
  KnobMenu* ptrKnob = KnobMenu::GetInstance();
  switch (keyValue) {
  case KEY_F1ADD: //F1
    ptrKnob->Knob1_Screw(1);
    break;
  case KEY_F1SUB: //F1
    ptrKnob->Knob1_Screw(0);
    break;
  case KEY_F2ADD: //F2
    ptrKnob->Knob2_Screw(1);
    break;
  case KEY_F2SUB: //F2
    ptrKnob->Knob2_Screw(0);
    break;
  case KEY_F3ADD: //F3
    ptrKnob->Knob3_Screw(1);
    break;
  case KEY_F3SUB: //F3
    ptrKnob->Knob3_Screw(0);
    break;
  case KEY_F4ADD: //F4
    ptrKnob->Knob4_Screw(1);
    break;
  case KEY_F4SUB: //F4
    ptrKnob->Knob4_Screw(0);
    break;
  case KEY_F5ADD: //F5
    ptrKnob->Knob5_Screw(1);
    break;
  case KEY_F5SUB: //F5
    ptrKnob->Knob5_Screw(0);
    break;
  }
}

void CKnobEvent::KnobEvent(unsigned char keyValue, unsigned char offset) {
  KnobMenu* ptrKnob = KnobMenu::GetInstance();

  switch (keyValue) {
  case KNOB_F1: //F1
    ptrKnob->Knob1_Screw(offset);
    break;
  case KNOB_F2: //F2
    ptrKnob->Knob2_Screw(offset);
    break;
  case KNOB_F3: //F3
    ptrKnob->Knob3_Screw(offset);
    break;
  case KNOB_F4: //F4
    ptrKnob->Knob4_Screw(offset);
    break;
  case KNOB_F5: // F5
    ptrKnob->Knob5_Screw(offset);
    break;
  default:
    break;
  }
}
