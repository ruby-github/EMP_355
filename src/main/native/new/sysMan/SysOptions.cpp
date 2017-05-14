#include "sysMan/SysOptions.h"

#include "Def.h"

SysOptions::SysOptions() {
  m_inifile = new IniFile(string(CFG_RES_PATH) + string(SYS_SETTING_FILE));
}

SysOptions::~SysOptions() {
  if (m_inifile != NULL) {
    delete m_inifile;
  }

  m_inifile = NULL;
}

int SysOptions::GetBiopsyLine() {
  return m_inifile->ReadInt("Options", "BiopsyLine");
}

int SysOptions::GetCenterLine() {
  return m_inifile->ReadInt("Options", "CenterLine");
}

int SysOptions::GetTI() {
  return m_inifile->ReadInt("Options", "TI");
}

int SysOptions::GetKeyboardSound() {
  return m_inifile->ReadInt("Options", "KeyboardSound");
}

int SysOptions::GetImageFormat() {
  return m_inifile->ReadInt("Options", "ImageFormat");
}

int SysOptions::GetDisplayFormatM() {
  return m_inifile->ReadInt("Options", "DisplayFormatM");
}

int SysOptions::GetDisplayFormatPW() {
  return m_inifile->ReadInt("Options", "DisplayFormatPW");
}

int SysOptions::GetImageMedia() {
  return m_inifile->ReadInt("Options", "ImageMedia");
}

int SysOptions::GetImageAutoName() {
  return m_inifile->ReadInt("Options", "ImageAutoName");
}

int SysOptions::GetCineFormat() {
  return m_inifile->ReadInt("Options", "CineFormat");
}

int SysOptions::GetCineMedia() {
  return m_inifile->ReadInt("Options", "CineMedia");
}

int SysOptions::GetCineAutoName() {
  return m_inifile->ReadInt("Options", "CineAutoName");
}

int SysOptions::GetMouseSpeed() {
  return m_inifile->ReadInt("Options", "MouseSpeed");
}

// 0-on 1-off
void SysOptions::SetBiopsyLine(int biopsyLine) {
  m_inifile->WriteInt("Options", "BiopsyLine", biopsyLine);
}

// 0-on 1-off
void SysOptions::SetCenterLine(int centerLine) {
  m_inifile->WriteInt("Options", "CenterLine", centerLine);
}

// 0-TIC 1-TIB 2-TIS
void SysOptions::SetTI(int TI) {
  m_inifile->WriteInt("Options", "TI", TI);
}

// 0-on 1-off
void SysOptions::SetKeyboardSound(int keyboardSound) {
  m_inifile->WriteInt("Options", "KeyboardSound", keyboardSound);
}

// 0-bmp 1-jpeg 2-emp
void SysOptions::SetImageFormat(int imageFormat) {
  m_inifile->WriteInt("Options", "ImageFormat", imageFormat);
}

// 0-harddisk 1-udisk
void SysOptions::SetDisplayFormatM(int displayFormat) {
  m_inifile->WriteInt("Options", "DisplayFormatM", displayFormat);
}

// 0-total 1-ud_11 2-ud_21 3-ud_12 4-lr_11
void SysOptions::SetDisplayFormatPW(int displayFormat) {
  m_inifile->WriteInt("Options", "DisplayFormatPW", displayFormat);
}

// 0-total 1-ud_11 2-ud_21 3-ud_12 4-lr_11
void SysOptions::SetImageMedia(int imageMedia) {
  m_inifile->WriteInt("Options", "ImageMedia", imageMedia);
}

// 0-manual 1-auto
void SysOptions::SetImageAutoName(int imageAutoName) {
  m_inifile->WriteInt("Options", "ImageAutoName", imageAutoName);
}

// 0-avi 1-cine
void SysOptions::SetCineFormat(int cineFormat) {
  m_inifile->WriteInt("Options", "CineFormat", cineFormat);
}

// 0-harddisk 1-udisk
void SysOptions::SetCineMedia(int cineMedia) {
  m_inifile->WriteInt("Options", "CineMedia", cineMedia);
}

// 0-manual 1-auto
void SysOptions::SetCineAutoName(int cineAutoName) {
  m_inifile->WriteInt("Options", "CineAutoName", cineAutoName);
}

// speed: 0-high 1-middle 2-low
void SysOptions::SetMouseSpeed(int mouseSpeed) {
  m_inifile->WriteInt("Options", "MouseSpeed", mouseSpeed);
}

void SysOptions::DefaultFactory() {
  int biopsyLine=1;
  int centerLine=1;
  int TI=2;
  int keyboardSound=0;
  int imageFormat=0;
  int imageMedia=0;
  int imageAutoName=1;
  int cineFormat=0;
  int cineMedia=0;
  int cineAutoName=1;
  int displayFormat=1;
  int mouseSpeed = 1;

  m_inifile->WriteInt("Options", "BiopsyLine", biopsyLine);
  m_inifile->WriteInt("Options", "CenterLine", centerLine);
  m_inifile->WriteInt("Options", "TI", TI);
  m_inifile->WriteInt("Options", "KeyboardSound", keyboardSound);
  m_inifile->WriteInt("Options", "ImageFormat", imageFormat);
  m_inifile->WriteInt("Options", "ImageMedia", imageMedia);
  m_inifile->WriteInt("Options", "ImageAutoName", imageAutoName);
  m_inifile->WriteInt("Options", "CineFormat", cineFormat);
  m_inifile->WriteInt("Options", "CineMedia", cineMedia);
  m_inifile->WriteInt("Options", "CineAutoName", cineAutoName);
  m_inifile->WriteInt("Options", "DisplayFormatPW", displayFormat);
  displayFormat=4;
  m_inifile->WriteInt("Options", "DisplayFormatM", displayFormat);
  m_inifile->WriteInt("Options", "MouseSpeed", mouseSpeed);
}

void SysOptions::SyncFile() {
  m_inifile->SyncConfigFile();
}
