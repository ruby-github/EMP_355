#include "utils/IniFile.h"

#include "sysMan/SysBiopsySetting.h"
#include "Def.h"

SysBiopsySetting::SysBiopsySetting() {
  m_inifile = new IniFile(string(CFG_RES_PATH) + string(SYS_SETTING_FILE));
}

SysBiopsySetting::~SysBiopsySetting() {
  if (m_inifile != NULL) {
    delete m_inifile;
  }

  m_inifile = NULL;
}

int SysBiopsySetting::GetBiopsyX_R50() {
  return m_inifile->ReadInt("BiopsySetting", "BiopsyX_R50");
}

int SysBiopsySetting::GetBiopsyAngle_R50() {
  return m_inifile->ReadInt("BiopsySetting", "BiopsyAngle");
}

int SysBiopsySetting::GetBiopsyX_T10() {
  return m_inifile->ReadInt("BiopsySetting", "BiopsyX_T10");
}

int SysBiopsySetting::GetBiopsyAngle_T10() {
  return m_inifile->ReadInt("BiopsySetting", "BiopsyAngle_T10");
}

int SysBiopsySetting::GetBiopsyX_L40() {
  return m_inifile->ReadInt("BiopsySetting", "BiopsyX_L40");
}

int SysBiopsySetting::GetBiopsyAngle_L40() {
  return m_inifile->ReadInt("BiopsySetting", "BiopsyAngle_L40");
}

int SysBiopsySetting::GetBiopsyX_R40() {
  return m_inifile->ReadInt("BiopsySetting", "BiopsyX_R40");
}

int SysBiopsySetting::GetBiopsyY_R40() {
  return m_inifile->ReadInt("BiopsySetting", "BiopsyY_R40");
}

int SysBiopsySetting::GetBiopsyX_R20() {
  return m_inifile->ReadInt("BiopsySetting", "BiopsyX_R20");
}

int SysBiopsySetting::GetBiopsyY_R20() {
  return m_inifile->ReadInt("BiopsySetting", "BiopsyY_R20");
}

int SysBiopsySetting::GetBiopsyX_T13() {
  return m_inifile->ReadInt("BiopsySetting", "BiopsyX_T13");
}

int SysBiopsySetting::GetBiopsyY_T13() {
  return m_inifile->ReadInt("BiopsySetting", "BiopsyY_T13");
}

void SysBiopsySetting::SetBiopsyX_R50(int BiopsyX) {
  m_inifile->WriteInt("BiopsySetting", "BiopsyX_R50", BiopsyX);
}

void SysBiopsySetting::SetBiopsyAngle_R50(int Angle) {
  m_inifile->WriteInt("BiopsySetting", "BiopsyAngle", Angle);
}

void SysBiopsySetting::SetBiopsyX_T10(int BiopsyX) {
  m_inifile->WriteInt("BiopsySetting", "BiopsyX_T10", BiopsyX);
}

void SysBiopsySetting::SetBiopsyAngle_T10(int Angle) {
  m_inifile->WriteInt("BiopsySetting", "BiopsyAngle_T10", Angle);
}

void SysBiopsySetting::SetBiopsyX_R40(int BiopsyX) {
  m_inifile->WriteInt("BiopsySetting", "BiopsyX_R40", BiopsyX);
}

void SysBiopsySetting::SetBiopsyY_R40(int BiopsyY) {
  m_inifile->WriteInt("BiopsySetting", "BiopsyY_R40", BiopsyY);
}

void SysBiopsySetting::SetBiopsyX_R20(int BiopsyX) {
  m_inifile->WriteInt("BiopsySetting", "BiopsyX_R20", BiopsyX);
}

void SysBiopsySetting::SetBiopsyY_R20(int BiopsyY) {
  m_inifile->WriteInt("BiopsySetting", "BiopsyY_R20", BiopsyY);
}

void SysBiopsySetting::SetBiopsyX_T13(int BiopsyX) {
  m_inifile->WriteInt("BiopsySetting", "BiopsyX_T13", BiopsyX);
}

void SysBiopsySetting::SetBiopsyY_T13(int BiopsyY) {
  m_inifile->WriteInt("BiopsySetting", "BiopsyY_T13", BiopsyY);
}

void SysBiopsySetting::SetBiopsyX_L40(int BiopsyX) {
  m_inifile->WriteInt("BiopsySetting", "BiopsyX_L40", BiopsyX);
}

void SysBiopsySetting::SetBiopsyAngle_L40(int Angle) {
  m_inifile->WriteInt("BiopsySetting", "BiopsyAngle_L40", Angle);
}

void SysBiopsySetting::SyncFile() {
  m_inifile->SyncConfigFile();
}
