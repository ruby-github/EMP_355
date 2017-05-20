#ifndef __BIOPSY_MAN_H__
#define __BIOPSY_MAN_H__

#include "probe/BiopsySetting.h"
#include "probe/DefaultBiopsySetting.h"

class BiopsyMan {
public:
  static BiopsyMan* GetInstance();

public:
  ~BiopsyMan();

  vector<string> GetBioBracketTypesOfCurProbe();
  vector<string> GetBioAngleTypesOfDefaultBioBracket();
  vector<string> GetBioAngleTypesOfcurBioBracket();

  string GetCurBioAngleType();
  string GetCurBioBracketType();
  string GetDefaultAngleTypeOfCurBioBracket();
  string GetDefaultBioBracketTypeOfCurProbe();
  int GetBioBracketTypeNumOfCurProbe();

  double GetCurBioAngleValue();
  double GetCurBioAngleValueDefault();
  double GetBioXOfCurBioBracketAngleType();
  double GetDefaultBioXOfCurBioBracketAngleType();

  void SetCurBioAngleType(string angleType);
  void SetCurBioBracketType(string bioBracketType);
  void SetCurBioBracketAndAngleTypeOfProbeChanged();
  void WriteXAngleValue(double xValue, double angleValue);

  bool IsBioBracketChanged();

private:
  BiopsyMan();

private:
  static BiopsyMan* m_instance;

private:
  string m_curProbeModel;
  string m_defaultBioBracketType;
  string m_curBioBracketType;
  string m_curBioAngleType;

  bool m_isBioBracketChanged;

  BiopsySetting* m_ptrBiopsySetting;
  DefaultBiopsySetting* m_ptrDefaultBiopsySetting;
};

#endif
