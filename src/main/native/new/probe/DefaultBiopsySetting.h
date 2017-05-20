#ifndef __DEFAULT_BIOPSY_SETTING_H__
#define __DEFAULT_BIOPSY_SETTING_H__

#include "utils/IniFile.h"

class DefaultBiopsySetting {
public:
  DefaultBiopsySetting();
  ~DefaultBiopsySetting();

public:
  double GetBiopsyXOfAngleType(const string bioBracketTypeAngle);
  double GetBiopsyAngleValueOfAngleType(const string bioBracketTypeAngle);

private:
  IniFile* m_ptrIni;
};

#endif
