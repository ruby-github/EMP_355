#include "sysMan/SysPrinterSetting.h"

#include "Def.h"
#include "periDevice/ViewPrintSetting.h"

SysPrinterSetting::SysPrinterSetting() {
  m_inifile = new IniFile(string(CFG_RES_PATH) + string(SYS_SETTING_FILE));
}

SysPrinterSetting::~SysPrinterSetting() {
  if (m_inifile != NULL) {
    delete m_inifile;
  }

  m_inifile = NULL;
}

int SysPrinterSetting::GetCopies() {
  return m_inifile->ReadInt("PrinterSetting", "Copies");
}

int SysPrinterSetting::GetGamma() {
  return m_inifile->ReadInt("PrinterSetting", "Gamma");
}

int SysPrinterSetting::GetBrightness() {
  return m_inifile->ReadInt("PrinterSetting", "Brightness");
}

int SysPrinterSetting::GetMediaSize() {
  return m_inifile->ReadInt("PrinterSetting", "MediaSize");
}

int SysPrinterSetting::GetLandscape() {
  return m_inifile->ReadInt("PrinterSetting", "Landscape");
}

int SysPrinterSetting::GetPageSize() {
  return m_inifile->ReadInt("PrinterSetting", "PageSize");
}

int SysPrinterSetting::GetSonyGamma() {
  return m_inifile->ReadInt("PrinterSetting", "SonyGamma");
}

void SysPrinterSetting::SetCopies(int copies) {
  m_inifile->WriteInt("PrinterSetting", "Copies", copies);
}

void SysPrinterSetting::SetGamma(int gamma) {
  m_inifile->WriteInt("PrinterSetting", "Gamma", gamma);
}

void SysPrinterSetting::SetBrightness(int brightness) {
  m_inifile->WriteInt("PrinterSetting", "Brightness", brightness);
}

void SysPrinterSetting::SetMediaSize(int media_size) {
  m_inifile->WriteInt("PrinterSetting", "MediaSize", media_size);
}

void SysPrinterSetting::SetLandscape(int landscape_ang) {
  m_inifile->WriteInt("PrinterSetting", "Landscape", landscape_ang);
}

void SysPrinterSetting::SetPageSize(int pagesize) {
  m_inifile->WriteInt("PrinterSetting", "PageSize", pagesize);
}

void SysPrinterSetting::SetSonyGamma(int sonygamma) {
  m_inifile->WriteInt("PrinterSetting", "SonyGamma", sonygamma);
}

void SysPrinterSetting::DefaultFactory() {
  if(ViewPrintSetting::GetInstance()->SonyURI()) {
    m_inifile->WriteInt("PrinterSetting", "Copies", 1);
    m_inifile->WriteInt("PrinterSetting", "Landscape", 0);
    m_inifile->WriteInt("PrinterSetting", "PageSize", 0);
    m_inifile->WriteInt("PrinterSetting", "SonyGamma", 0);
  } else {
    m_inifile->WriteInt("PrinterSetting", "Copies", 1);
    m_inifile->WriteInt("PrinterSetting", "Gamma", 1000);
    m_inifile->WriteInt("PrinterSetting", "Brightness", 100);
    m_inifile->WriteInt("PrinterSetting", "MediaSzie", 0);
  }
}

void SysPrinterSetting::SyncFile() {
  m_inifile->SyncConfigFile();
}
