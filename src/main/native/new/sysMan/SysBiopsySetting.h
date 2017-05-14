#ifndef __SYS_BIOPSY_SETTING_H__
#define __SYS_BIOPSY_SETTING_H__

#include "utils/IniFile.h"

class SysBiopsySetting {
public:
  SysBiopsySetting();
  ~SysBiopsySetting();

  int GetBiopsyX_R50();
  int GetBiopsyAngle_R50();
  int GetBiopsyX_R40();
  int GetBiopsyY_R40();
  int GetBiopsyX_R20();
  int GetBiopsyY_R20();
  int GetBiopsyX_T10();
  int GetBiopsyAngle_T10();
  int GetBiopsyX_T13();
  int GetBiopsyY_T13();
  int GetBiopsyX_L40();
  int GetBiopsyAngle_L40();

  void SetBiopsyX_T10(int BiopsyX);
  void SetBiopsyAngle_T10(int Angle);
  void SetBiopsyX_R50(int BiopsyX);
  void SetBiopsyAngle_R50(int Angle);
  void SetBiopsyX_R40(int BiopsyX);
  void SetBiopsyY_R40(int BiopsyY);
  void SetBiopsyX_R20(int BiopsyX);
  void SetBiopsyY_R20(int BiopsyY);
  void SetBiopsyX_T13(int BiopsyX);
  void SetBiopsyY_T13(int BiopsyY);
  void SetBiopsyX_L40(int BiopsyX);
  void SetBiopsyAngle_L40(int Angle);

  void SyncFile();
private:
  IniFile* m_inifile;
};

#endif
