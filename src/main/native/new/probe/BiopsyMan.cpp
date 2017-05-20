#include "probe/BiopsyMan.h"

#include "probe/ProbeMan.h"
#include "probe/ProbeSocket.h"

BiopsyMan* BiopsyMan::m_instance = NULL;

// ---------------------------------------------------------

BiopsyMan* BiopsyMan::GetInstance() {
  if (m_instance == NULL) {
    m_instance = new BiopsyMan();
  }

  return m_instance;
}

BiopsyMan::BiopsyMan() {
  m_ptrBiopsySetting = new BiopsySetting();
  m_ptrDefaultBiopsySetting = new DefaultBiopsySetting();

  ProbeSocket::ProbePara curPara;
  ProbeMan::GetInstance()->GetCurProbe(curPara);

  m_curProbeModel = curPara.model;
  m_curProbeModel += curPara.factoryFlag;
  m_defaultBioBracketType = m_ptrBiopsySetting->GetDefaultBioBracketType(m_curProbeModel);
  m_curBioBracketType = m_defaultBioBracketType;
  m_curBioAngleType = m_ptrBiopsySetting->GetDefaultAngleType(m_curBioBracketType);
  m_isBioBracketChanged = false;
}

BiopsyMan::~BiopsyMan() {
  if (m_instance != NULL) {
    delete m_instance;
  }

  m_instance = NULL;

  if (m_ptrBiopsySetting != NULL) {
    delete m_ptrBiopsySetting;

    m_ptrBiopsySetting = NULL;
  }

  if (m_ptrDefaultBiopsySetting != NULL) {
    delete m_ptrDefaultBiopsySetting;

    m_ptrDefaultBiopsySetting = NULL;
  }
}

vector<string> BiopsyMan::GetBioBracketTypesOfCurProbe() {
  ProbeSocket::ProbePara curPara;
  ProbeMan::GetInstance()->GetCurProbe(curPara);
  m_curProbeModel = curPara.model;
  m_curProbeModel += curPara.factoryFlag;

  return m_ptrBiopsySetting->GetBioBracketTypeOfProbe(m_curProbeModel);
}

vector<string> BiopsyMan::GetBioAngleTypesOfDefaultBioBracket() {
  return m_ptrBiopsySetting->GetBioBracketAngleType(m_defaultBioBracketType);
}

vector<string> BiopsyMan::GetBioAngleTypesOfcurBioBracket() {
  return m_ptrBiopsySetting->GetBioBracketAngleType(m_curBioBracketType);
}

string BiopsyMan::GetCurBioAngleType() {
  return m_curBioAngleType;
}

string BiopsyMan::GetCurBioBracketType() {
  return m_curBioBracketType;
}

string BiopsyMan::GetDefaultBioBracketTypeOfCurProbe() {
  ProbeSocket::ProbePara curPara;
  ProbeMan::GetInstance()->GetCurProbe(curPara);
  m_curProbeModel = curPara.model;
  m_curProbeModel += curPara.factoryFlag;

  m_defaultBioBracketType=m_ptrBiopsySetting->GetDefaultBioBracketType(m_curProbeModel);

  return m_defaultBioBracketType;
}

string BiopsyMan::GetDefaultAngleTypeOfCurBioBracket() {
  return m_ptrBiopsySetting->GetDefaultAngleType(m_curBioBracketType);
}

int BiopsyMan::GetBioBracketTypeNumOfCurProbe() {
  ProbeSocket::ProbePara curPara;
  ProbeMan::GetInstance()->GetCurProbe(curPara);
  m_curProbeModel = curPara.model;
  m_curProbeModel += curPara.factoryFlag;

  return m_ptrBiopsySetting->GetBioBracketNum(m_curProbeModel);
}

double BiopsyMan::GetCurBioAngleValue() {
  string bracketAngle = m_curBioBracketType + "-" + m_curBioAngleType;

  return m_ptrBiopsySetting->GetBiopsyAngleValueOfAngleType(bracketAngle);
}

double  BiopsyMan::GetCurBioAngleValueDefault() {
  string bracketAngle = m_curBioBracketType + "-" + m_curBioAngleType;

  return (m_ptrDefaultBiopsySetting->GetBiopsyAngleValueOfAngleType(bracketAngle));
}

double  BiopsyMan::GetBioXOfCurBioBracketAngleType() {
  string bracketAngle = m_curBioBracketType + "-" + m_curBioAngleType;

  return m_ptrBiopsySetting->GetBiopsyXOfAngleType(bracketAngle);
}

double BiopsyMan::GetDefaultBioXOfCurBioBracketAngleType() {
  string bracketAngle = m_curBioBracketType + "-" + m_curBioAngleType;

  return m_ptrDefaultBiopsySetting->GetBiopsyXOfAngleType(bracketAngle);
}

void BiopsyMan::SetCurBioAngleType(string angleType) {
  m_curBioAngleType = angleType;
}

void BiopsyMan::SetCurBioBracketType(string bioBracketType) {
  if(m_curBioBracketType.compare(bioBracketType) != 0) {
    m_isBioBracketChanged =true;
  } else {
    m_isBioBracketChanged = false;
  }

  m_curBioBracketType=bioBracketType;
}

void BiopsyMan::SetCurBioBracketAndAngleTypeOfProbeChanged() {
  m_curBioBracketType = GetDefaultBioBracketTypeOfCurProbe();
  m_curBioAngleType = GetDefaultAngleTypeOfCurBioBracket();
}

void BiopsyMan::WriteXAngleValue(double xValue, double angleValue) {
  string bracketAngleType = m_curBioBracketType + "-" + m_curBioAngleType;

  m_ptrBiopsySetting->SetBiopsyAngleValueOfAngleType(bracketAngleType,angleValue);
  m_ptrBiopsySetting->SetBiopsyXOfAngleType(bracketAngleType,xValue);
  m_ptrBiopsySetting->SyncFile();
}

bool BiopsyMan::IsBioBracketChanged() {
  return m_isBioBracketChanged;
}
