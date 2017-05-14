#ifndef __SYS_CALCULATE_SETTING_H__
#define __SYS_CALCULATE_SETTING_H__

#include "utils/IniFile.h"

class SysCalculateSetting {
public:
  SysCalculateSetting();
  ~SysCalculateSetting();

public:
  int GetBSAMethod();
  int GetCerMethod();
  int GetHlMethod();
  int GetBpdMethod();
  int GetFlMethod();
  int GetCrlMethod();
  int GetGsMethod();
  int GetAcMethod();
  int GetHcMethod();
  int GetTadMethod();
  int GetApadMethod();
  int GetThdMethod();
  int GetOfdMethod();
  int GetEfwMethod();
  int GetCurEfwMethod();
  int GetGWMethod(int item);

  void SetBSAMethod(int BSA);
  void SetCerMethod(int cer);
  void SetHlMethod(int hl);
  void SetBpdMethod(int bpd);
  void SetFlMethod(int fl);
  void SetCrlMethod(int crl);
  void SetGsMethod(int gs);
  void SetAcMethod(int ac);
  void SetHcMethod(int hc);
  void SetTadMethod(int tad);
  void SetApadMethod(int apad);
  void SetThdMethod(int thd);
  void SetOfdMethod(int ofd);
  void SetEfwMethod(int efw);

  void DefaultFactory();
  void UpdateOBSetting();
  void SyncFile();

private:
  IniFile* m_inifile;
};

#endif
