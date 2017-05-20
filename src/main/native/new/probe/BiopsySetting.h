#ifndef __BIOPSY_SETTING_H__
#define __BIOPSY_SETTING_H__

#include "utils/IniFile.h"

class BiopsySetting {
public:
  BiopsySetting();
  ~BiopsySetting();

public:
  string GetDefaultAngleType(const string bioBracketType);
  string GetDefaultBioBracketType(const string probeModel);
  int GetBioBracketNum(const string probeModel);
  int GetBioBracketAngleNum(const string bioBracketType);
  double GetBiopsyXOfAngleType(const string bioBracketTypeAngle);
  double GetBiopsyAngleValueOfAngleType(const string bioBracketTypeAngle);

  vector<string> GetBioBracketAngleType(const string bioBracketType);
  vector<string> GetBioBracketTypeOfProbe(const string probeModel);

  void SetBiopsyXOfAngleType(const string bioBracketTypeAngle, double value);
  void SetBiopsyAngleValueOfAngleType(const string bioBracketTypeAngle, double value);

  void SyncFile();

private:
  IniFile* m_ptrIni;
};

#endif
