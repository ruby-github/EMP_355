#include "sysMan/SysNoteSetting.h"

#include "Def.h"

SysNoteSetting::SysNoteSetting() {
  m_inifile = new IniFile(string(CFG_RES_PATH) + string(SYS_SETTING_FILE));
}

SysNoteSetting::~SysNoteSetting() {
  if (m_inifile != NULL) {
    delete m_inifile;
  }

  m_inifile = NULL;
}

int SysNoteSetting::GetFontSize() {
  return m_inifile->ReadInt("NoteSetting", "FontSize");
}

int SysNoteSetting::GetBodyMarkSize() {
  return m_inifile->ReadInt("NoteSetting", "BodyMarkSize");
}

int SysNoteSetting::GetFontColor() {
  return m_inifile->ReadInt("NoteSetting", "FontColor");
}

int SysNoteSetting::GetBodyMarkColor() {
  return m_inifile->ReadInt("NoteSetting", "BodyMarkColor");
}

// fontSize: 0-big 1-normal 2-small
void SysNoteSetting::SetFontSize(int fontSize) {
  m_inifile->WriteInt("NoteSetting", "FontSize", fontSize);
}

// bodyMark: 0-big 1-mid 2-small
void SysNoteSetting::SetBodyMarkSize(int bodyMarkSize) {
  m_inifile->WriteInt("NoteSetting", "BodyMarkSize", bodyMarkSize);
}

void SysNoteSetting::SetFontColor(int fontColor) {
  m_inifile->WriteInt("NoteSetting", "FontColor", fontColor);
}

// bodyMarkColor: 0-white, 1-gray, 2-red, 3-green, 4-yellow, 5-blue
void SysNoteSetting::SetBodyMarkColor(int bodyMarkColor) {
  m_inifile->WriteInt("NoteSetting", "BodyMarkColor", bodyMarkColor);
}

void SysNoteSetting::DefaultFactory() {
  int fontSize = 1;
  int bodyMarkSize = 1;
  int fontColor = 1;
  int bodyMarkColor = 1;

  m_inifile->WriteInt("NoteSetting", "FontSize", fontSize);
  m_inifile->WriteInt("NoteSetting", "BodyMarkSize", bodyMarkSize);
  m_inifile->WriteInt("NoteSetting", "FontColor", fontColor);
  m_inifile->WriteInt("NoteSetting", "BodyMarkColor", bodyMarkColor);
}

void SysNoteSetting::SyncFile() {
  m_inifile->SyncConfigFile();
}
