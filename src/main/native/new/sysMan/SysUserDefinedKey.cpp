#include "sysMan/SysUserDefinedKey.h"

#include "Def.h"

SysUserDefinedKey::SysUserDefinedKey() {
  m_inifile = new IniFile(string(CFG_RES_PATH) + string(SYS_SETTING_FILE));
}

SysUserDefinedKey::~SysUserDefinedKey() {
  if (m_inifile != NULL) {
    delete m_inifile;
  }

  m_inifile = NULL;
}

int SysUserDefinedKey::GetFuncP1() {
  return m_inifile->ReadInt("UserDefinedKey", "P1");
}

int SysUserDefinedKey::GetFuncP2() {
  return m_inifile->ReadInt("UserDefinedKey", "P2");
}

int SysUserDefinedKey::GetFuncP3() {
  return m_inifile->ReadInt("UserDefinedKey", "P3");
}

void SysUserDefinedKey::SetFuncP1(int func) {
  m_inifile->WriteInt("UserDefinedKey", "P1", func);
}

void SysUserDefinedKey::SetFuncP2(int func) {
  m_inifile->WriteInt("UserDefinedKey", "P2", func);
}

void SysUserDefinedKey::SetFuncP3(int func) {
  m_inifile->WriteInt("UserDefinedKey", "P3", func);
}

void SysUserDefinedKey::SyncFile() {
  m_inifile->SyncConfigFile();
}
