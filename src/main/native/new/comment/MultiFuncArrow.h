#ifndef __MULTI_FUNC_ARROW_H__
#define __MULTI_FUNC_ARROW_H__

#include "utils/CalcTime.h"

#include "Def.h"
#include "AbsMultiFunc.h"
#include "AbsUpdateArrow.h"
#include "comment/Arrow.h"
#include "display/KnobMenu.h"
#include "keyboard/KnobEvent.h"

class MultiFuncArrow: public CAbsUpdateArrow, public CKnobEvent, public AbsMultiFunc {
public:
  static EKnobReturn PressSetShape();
  static EKnobReturn SetShape(EKnobOper opr);
  static EKnobReturn PressSetSize();
  static EKnobReturn SetSize(EKnobOper opr);
  static EKnobReturn PressSetColor();
  static EKnobReturn SetColor(EKnobOper opr);

public:
  MultiFuncArrow();
  virtual ~MultiFuncArrow();

public:
  virtual void Do();
  virtual void Undo();
  virtual void Value(EKnobOper opr);
  virtual void Mouse(int offsetX, int offsetY);
  virtual void Esc();

  virtual void KeyEvent(unsigned char keyValue);
  virtual void KnobEvent(unsigned char keyValue, unsigned char offset);
  virtual void MouseEvent(char offsetX, char offsetY);
  virtual void KnobArrowCreate();

  virtual void DrawArrow(POINT pos, unsigned int direction,
    double scale, unsigned int color, unsigned int shape);

  POINT GetPoint(int offsetX, int offsetY);

protected:
  enum PRESSMODE {
    CLICK, DBLCLK
  };

  enum ARROWOPR {
    ADD, MODIFY
  };

private:
  // signal
  static gboolean signal_callback_clicked(gpointer data) {
    if (m_pThis == NULL) {
      return FALSE;
    }

    m_pThis->Clicked();

    return FALSE;
  }

private:
  static EKnobReturn GetKnobRetShape(CArrow::SHAPE shape);
  static EKnobReturn GetKnobRetSize(CArrow::SIZE size);
  static EKnobReturn GetKnobRetColor(CArrow::COLOR color);

private:
  void Clicked();

private:
  static MultiFuncArrow* m_pThis;

private:
  CArrow* m_arrow;

  KnobMenu::KnobItem* m_knobItemBku;
  KnobMenu::EKnobType m_knobTypeBku;

  PRESSMODE m_pressMode;
  ARROWOPR m_arrowOpr;
  POINT m_pos;
  int m_timer;
};

#endif
