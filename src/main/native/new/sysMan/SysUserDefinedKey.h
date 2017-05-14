#ifndef __SYS_USER_DEFINED_KEY_H__
#define __SYS_USER_DEFINED_KEY_H__

#include "utils/IniFile.h"

class SysUserDefinedKey {
public:
  SysUserDefinedKey();
  ~SysUserDefinedKey();

public:
  int GetFuncP1();
  int GetFuncP2();
  int GetFuncP3();

  void SetFuncP1(int func);
  void SetFuncP2(int func);
  void SetFuncP3(int func);

  void SyncFile();

private:
  IniFile* m_inifile;
};

#endif
