#ifndef __MULTI_FUNC_BODYMARK_H__
#define __MULTI_FUNC_BODYMARK_H__

#include "utils/FakeXEvent.h"
#include "utils/Utils.h"

#include "AbsMultiFunc.h"
#include "bodymark/BodyMark.h"

class MultiFuncBodyMark: public FakeXEvent, public AbsMultiFunc {
public:
  static void SetNewBodyMark(GdkPixbuf* pixbuf);

  static EKnobReturn SetBodyMarkSize(EKnobOper opr);
  static EKnobReturn SetProbeMarkColor(EKnobOper opr);
  static EKnobReturn PressSetBodyMarkSize();
  static EKnobReturn PressSetProbeMarkColor();
  static EKnobReturn PressSetHideBodyMark();
  static EKnobReturn PressSetHideBodyMark(EKnobOper opr);

public:
  MultiFuncBodyMark();
  ~MultiFuncBodyMark();

public:
  void KeyEvent(unsigned char keyValue);
  void KnobEvent(unsigned char keyValue, unsigned char offset);
  void MouseEvent(char offsetX, char offsetY);
  void KnobBodyMarkCreate();

private:
  static EKnobReturn GetKnobRetBodyMarkSize(BodyMark::SIZE size);
  static EKnobReturn GetKnobRetProbeMarkColor(BodyMark::COLOR color);

private:
  void BeginMoveProbeMark();
  void EndMoveProbeMark();

  void EndMoveBodyMark();
  void UndoMoveBodyMark();

  void AddNewBodyMark(GdkPixbuf* pixbuf);
  void GetBodyMarkSize(int& w, int& h);
  void ReviseProbePos(double scale);

  void Esc();
  void Hide();
  bool Clicked();
  bool DClicked();

  POINT GetPointProbe(int offsetX, int offsetY);
  POINT GetPointBody(int offsetX, int offsetY);

  void Value(EKnobOper opr);

private:
  static MultiFuncBodyMark* m_this;

private:
  enum BODYMARK_OPR {
    IDEL, ADD, MOVE_PROBE, MOVE_BODY
  };

  POINT m_bodyPos[4];           // pos of left-up corner of bodymark
  POINT m_probePos[4];          // pos of center of probe mark
  POINT m_probePosOffset;       // offset of probe mark to body mark(left-up corner)
  GdkRectangle m_bodyPosRange;  // range of bodymark's pos
  BodyMark* m_ptrBodyMark;
  BODYMARK_OPR m_bodyOpr;

  int m_timer;
  int m_active;
};

#endif
