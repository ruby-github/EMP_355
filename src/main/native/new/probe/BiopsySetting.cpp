#include "probe/BiopsySetting.h"

#include <sstream>
#include "Def.h"

using namespace std;

BiopsySetting::BiopsySetting() {
  m_ptrIni = new IniFile(string(CFG_RES_PATH) + string(BIOPSY_SETTING_FILE));
}

BiopsySetting::~BiopsySetting() {
  if (m_ptrIni != NULL) {
    delete m_ptrIni;

    m_ptrIni = NULL;
  }
}

string BiopsySetting::GetDefaultAngleType(const string bioBracketType) {
  return m_ptrIni->ReadString(bioBracketType, "DefaultAngleType");
}

string BiopsySetting::GetDefaultBioBracketType(const string probeModel) {
  return m_ptrIni->ReadString(probeModel, "DefaultBracket");
}

int BiopsySetting::GetBioBracketNum(const string probeModel) {
return m_ptrIni->ReadInt(probeModel, "Num");
}

int BiopsySetting::GetBioBracketAngleNum(const string bioBracketType) {
  return m_ptrIni->ReadInt(bioBracketType, "Num");
}

double BiopsySetting::GetBiopsyXOfAngleType(const string bioBracketTypeAngle) {
  return m_ptrIni->ReadDouble(bioBracketTypeAngle, "BiopsyX");
}

double BiopsySetting::GetBiopsyAngleValueOfAngleType(const string bioBracketTypeAngle) {
  return m_ptrIni->ReadDouble(bioBracketTypeAngle, "BiopsyAngle");
}

vector<string> BiopsySetting::GetBioBracketAngleType(const string bioBracketType) {
  vector<string> vec;

  int num = GetBioBracketAngleNum(bioBracketType);

  for(int i = 0; i < num; i++) {
    stringstream ss;
    ss << "AngleType" << i;

    string angleType = m_ptrIni->ReadString(bioBracketType,ss.str());
    vec.push_back(angleType);
  }

  return vec;
}

vector<string> BiopsySetting::GetBioBracketTypeOfProbe(const string probeModel) {
  vector<string> vec;

  int num = GetBioBracketNum(probeModel);

  for(int i=0; i<num; i++) {
    stringstream ss;
    ss << "Bracket" << i;

    string bracketType = m_ptrIni->ReadString(probeModel, ss.str());
    vec.push_back(bracketType);
  }

  return vec;
}

void BiopsySetting::SetBiopsyXOfAngleType(const string bioBracketTypeAngle, double value) {
  m_ptrIni->WriteDouble(bioBracketTypeAngle, "BiopsyX", value);
}

void BiopsySetting::SetBiopsyAngleValueOfAngleType(const string bioBracketTypeAngle, double value) {
  m_ptrIni->WriteDouble(bioBracketTypeAngle, "BiopsyAngle", value);
}

void BiopsySetting::SyncFile() {
  m_ptrIni->SyncConfigFile();
}

#include "Def.h"
