#ifndef __MULTI_FUNC_VALUE_H__
#define __MULTI_FUNC_VALUE_H__

#include "Def.h"
#include "AbsMultiFunc.h"
#include "measure/MeasureDraw.h"

class MultiFuncFocus: public AbsMultiFunc {
public:
  MultiFuncFocus();
  ~MultiFuncFocus();

public:
  void Value(EKnobOper opr);
  void Mouse(int offsetX, int offsetY);
  void Undo();
  void KeyUp();
  void KeyDown();
};

class MultiFuncDepth: public AbsMultiFunc {
public:
  MultiFuncDepth();
  ~MultiFuncDepth();

public:
  void Value(EKnobOper opr);
  void Undo();
};

class MultiFuncLocalZoom: public AbsMultiFunc {
public:
  MultiFuncLocalZoom();
  ~MultiFuncLocalZoom();

public:
  void Do();
  void Undo();
  void Update();
  void Mouse(int offsetX, int offsetY);   // change sample box's position
  void KeyLeft();
  void KeyRight();
  void KeyUp();
  void KeyDown();

private:
  int n1;
  bool m_changeSize;  // true: change size, false: move pos
};

class MultiFuncGlobalZoom: public AbsMultiFunc {
public:
  MultiFuncGlobalZoom();
  ~MultiFuncGlobalZoom();

public:
  void Value(EKnobOper opr);            // change scale
  void Mouse(int offsetX, int offsetY); // change sample box's position
  void Undo();
  void KeyLeft();
  void KeyRight();
  void KeyUp();
  void KeyDown();
};

class MultiFuncPIPZoom: public AbsMultiFunc {
public:
  MultiFuncPIPZoom();
  ~MultiFuncPIPZoom();

public:
  void Value(EKnobOper opr);
  void Mouse(int offsetX, int offsetY);
  void Undo();
  void KeyLeft();
  void KeyRight();
  void KeyUp();
  void KeyDown();
};

class MultiFuncMeasure: public AbsMultiFunc {
public:
  MultiFuncMeasure();
  ~MultiFuncMeasure();

public:
  void Undo();
  void Do();
  void Update();
  void Value(EKnobOper opr);
  void Mouse(int offsetX, int offsetY);
  void KeyLeft();
  void KeyRight();
  void KeyUp();
  void KeyDown();

private:
  POINT m_p;
  MeasureDraw m_draw;

  POINT GetPoint(int offsetX, int offsetY);
};

class MultiFuncText: public AbsMultiFunc {
public:
  MultiFuncText();
  ~MultiFuncText();

public:
  void Undo();
};

class MultiFuncGrayTrans: public AbsMultiFunc {
public:
  MultiFuncGrayTrans();
  ~MultiFuncGrayTrans();

public:
  void Mouse(int offsetX, int offsetY);
  void Value(EKnobOper opr);
  void Do();
  void Undo();
  void Update();
  void KeyUp();
  void KeyDown();
};

class MultiFuncGrayReject: public AbsMultiFunc {
public:
  MultiFuncGrayReject();
  ~MultiFuncGrayReject();

public:
  void Mouse(int offsetX, int offsetY);
  void Undo();
  void KeyLeft();
  void KeyRight();
};

class MultiFuncFreq: public AbsMultiFunc {
public:
  MultiFuncFreq();
  ~MultiFuncFreq();

public:
  void Undo();
};

class MultiFuncChroma: public AbsMultiFunc {
public:
  MultiFuncChroma();
  ~MultiFuncChroma();

public:
  void Undo();
};

class MultiFuncBiopsyVerify: public AbsMultiFunc {
public:
  MultiFuncBiopsyVerify();
  ~MultiFuncBiopsyVerify();

public:
  void Value(EKnobOper opr);
  void Mouse(int offsetX, int offsetY);
  void Undo();
  void Do();

  void KeyLeft();
  void KeyRight();
  void KeyUp();
  void KeyDown();
};


class MultiFuncBiopsy: public AbsMultiFunc {
public:
  void Mouse(int offsetX, int offsetY);
  void Undo();
  void Do();
};

#endif
