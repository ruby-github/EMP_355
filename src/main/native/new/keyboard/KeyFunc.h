#ifndef __KEY_FUNC_H__
#define __KEY_FUNC_H__

#include "AbsCommand.h"
#include "probe/ExamItem.h"
#include "probe/ProbeSocket.h"

class KeySwitchIM: public AbsCommand {
public:
  bool Execute();
  bool ExcuteChange(bool isswitch);

public:
  // true: input method on; false: input method off
  static bool m_imOn;
};

class KeyPowerOff: public AbsCommand {
public:
  bool Execute();
};

class KeyTSI: public AbsCommand {
public:
  bool Execute();

private:
  bool Do();
  bool Undo();

public:
  static int keyNum;
};

class KeyAutoOptimize: public AbsCommand {
public:
  static void AutoOff();

public:
  bool Execute();

private:
  bool Do();
  bool Undo();
  void ImgOptimize(ProbeSocket::ProbePara p, ExamItem::ParaItem i);
  void BackupPara();

private:
  // true: enter optimize; false: exit optimize
  static bool m_autoOn;
  static ExamItem::ParaItem m_itemPara;
};

class KeyMeasure: public AbsCommand {
public:
  bool Execute();
};

class KeyCalc: public AbsCommand {
public:
  bool Execute();
};

class KeySystem: public AbsCommand {
public:
  bool Execute();
};

class KeyCursor: public AbsCommand {
public:
  bool Execute();
};

class KeyText: public AbsCommand {
public:
  bool Execute();
};

class KeyArrow: public AbsCommand {
public:
  bool Execute();
};

class KeyBodyMark: public AbsCommand {
public:
  bool Execute();
};

class KeySaveSnap: public AbsCommand {
public:
  bool Execute();
};

class KeyReview: public AbsCommand {
public:
  static void HideMenuReview();
  static void SetDefaultIDAndPath();
  static void SetDemoIDAndPath();
  static void ExitMenuReivew();

public:
  bool Execute();

public:
  static bool m_menuRead;
};

class KeyPIP: public AbsCommand {
public:
  bool Execute();
};

class KeyLocalZoom: public AbsCommand {
public:
  bool Execute();
  bool ExitLocalZoom();
};

class KeyFocus: public AbsCommand {
public:
  bool Execute();
};

class KeyDepth: public AbsCommand {
public:
  bool ExecuteAdd();
  bool ExecuteSub();
  bool Execute();

private:
  void DepthExe(EKnobOper oper);
};

class KeyFreq: public AbsCommand {
public:
  bool Execute();
};

class KeyChroma: public AbsCommand {
public:
  bool Execute();
};

class KeyClearScreen: public AbsCommand {
public:
  bool Execute();
  bool UnFreezeExecute(bool update=false);
  bool ModeExecute(bool update=false);

private:
  void DeleteDataForClearScreen();
};

class KeyEndExam: public AbsCommand {
public:
  bool Execute();
};

class KeyCenterLine: public AbsCommand {
public:
  bool Execute();
  void Clear();
};

class KeyMenu: public AbsCommand {
public:
  bool Execute();
};

class KeyBiopsy: public AbsCommand {
public:
  bool Execute();
};

void ChangeTis();
void DarkFucusLight();

#endif
