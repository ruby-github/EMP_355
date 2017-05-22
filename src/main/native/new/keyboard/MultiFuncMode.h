#ifndef __MULTI_FUNC_MODE_H__
#define __MULTI_FUNC_MODE_H__

#include "AbsMultiFunc.h"
#include "imageControl/CalcPw.h"
#include "imageProc/MenuPW.h"

void OverturnMouseOpr(int &offsetX, int &offsetY);

class MultiFuncNone: public AbsMultiFunc {
public:
  void Do();
  void Mouse(int offsetX, int offsetY);
  void KeyLeft();
  void KeyRight();
  void KeyUp();
  void KeyDown();
  void Update();
  void Value(EKnobOper opr);
  void Undo();
};

class MultiFuncReplay: public AbsMultiFunc {
public:
  MultiFuncReplay();

public:
  void Update();
  void Mouse(int offsetX, int offsetY);
  void Do();
  void KeyLeft();
  void KeyRight();

private:
  int m_count;
};

class MultiFuncMInit: public AbsMultiFunc {
public:
  void Update();
  void Mouse(int offsetX, int offsetY);
  void KeyLeft();
  void KeyRight();
};

class MultiFuncM: public AbsMultiFunc {
public:
  void Mouse(int offsetX, int offsetY);
  void KeyLeft();
  void KeyRight();
};

class MultiFuncPwInit: public AbsMultiFunc {
public:
  void Update();
  void Mouse(int offsetX, int offsetY);
  void KeyLeft();
  void KeyRight();
  void KeyUp();
  void KeyDown();
  void Value(EKnobOper oper);
};

class MultiFuncPw: public AbsMultiFunc {
public:
  void Update();
  void Mouse(int offsetX, int offsetY);
  void KeyLeft();
  void KeyRight();
  void KeyUp();
  void KeyDown();
  void Value(EKnobOper oper);
};

class MultiFuncCfm: public AbsMultiFunc {
public:
  void Do();
  void Mouse(int offsetX, int offsetY);
  void KeyLeft();
  void KeyRight();
  void KeyUp();
  void KeyDown();
};

class MultiFuncPwCfmInit: public AbsMultiFunc {
public:
  MultiFuncPwCfmInit();

public:
  void Do();
  virtual void Update();
  void Mouse(int offsetX, int offsetY);
  void KeyLeft();
  void KeyRight();
  void KeyUp();
  void KeyDown();
  void Value(EKnobOper oper);

private:
  char m_change;  // 0-SV; 1-BOX pos; 2-BOX size
};

class MultiFuncPwCfm: public MultiFuncPwCfmInit {
public:
  void Update();
};

class MultiFuncPwPdiInit: public MultiFuncPwCfmInit {
public:
  void Update();
};

class MultiFuncPwPdi: public MultiFuncPwCfmInit {
public:
  void Update();
};

class MultiFuncAnatomicM: public AbsMultiFunc {
public:
  MultiFuncAnatomicM();
  ~MultiFuncAnatomicM();

public:
  void Mouse(int offsetX, int offsetY);
  void Do();
  void KeyUp();
  void KeyDown();
  void KeyLeft();
  void KeyRight();

private:
  bool m_moveStatus;
};

class MultiFuncEFOV: public AbsMultiFunc {
public:
  MultiFuncEFOV();
  ~MultiFuncEFOV();

public:
  void Update();
  void Mouse(int offsetX, int offsetY);
  void Undo();
  void Do();
  void KeyLeft();
  void KeyRight();
};

class MultiFuncCwInit: public AbsMultiFunc {
public:
  void Update();
  void Mouse(int offsetX, int offsetY);
  void KeyLeft();
  void KeyRight();
  void KeyUp();
  void KeyDown();

public:
  CalcPw* m_ptrCalc;
};

class MultiFuncCw: public AbsMultiFunc {
public:
  void Update();
  void Mouse(int offsetX, int offsetY);
  void KeyLeft();
  void KeyRight();
  void KeyUp();
  void KeyDown();

public:
  CalcPw* m_ptrCalc;
};

class MultiFuncCwCfmInit: public AbsMultiFunc {
public:
  MultiFuncCwCfmInit();

public:
  void Do();
  virtual void Update();
  void Mouse(int offsetX, int offsetY);
  void KeyLeft();
  void KeyRight();
  void KeyUp();
  void KeyDown();

public:
  CalcPw* m_ptrCalc;

private:
  char m_change;  // 0-SV; 1-BOX pos; 2-BOX size
};

class MultiFuncCwCfm: public MultiFuncCwCfmInit {
public:
  void Update();
};

class MultiFuncCwPdiInit: public MultiFuncCwCfmInit {
public:
  void Update();
};

class MultiFuncCwPdi: public MultiFuncCwCfmInit {
public:
  void Update();

private:
  CalcPw* m_ptrCalc;
};

#endif
