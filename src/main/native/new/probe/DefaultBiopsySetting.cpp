#include "probe/DefaultBiopsySetting.h"

#include "Def.h"

DefaultBiopsySetting::DefaultBiopsySetting() {
  m_ptrIni = new IniFile(string(CFG_RES_PATH) + string(DEFAULT_SETTING_FILE));
}

DefaultBiopsySetting::~DefaultBiopsySetting() {
  if (m_ptrIni != NULL) {
    delete m_ptrIni;
    m_ptrIni = NULL;
  }
}

double DefaultBiopsySetting::GetBiopsyXOfAngleType(const string bioBracketTypeAngle) {
  return m_ptrIni->ReadDouble(bioBracketTypeAngle, "BiopsyX");
}

double  DefaultBiopsySetting::GetBiopsyAngleValueOfAngleType(const string bioBracketTypeAngle) {
  return m_ptrIni->ReadDouble(bioBracketTypeAngle, "BiopsyAngle");
}
