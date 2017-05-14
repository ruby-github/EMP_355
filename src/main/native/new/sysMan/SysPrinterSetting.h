#ifndef __SYS_PRINTER_SETTING_H__
#define __SYS_PRINTER_SETTING_H__

#include "utils/IniFile.h"

class SysPrinterSetting {
public:
  SysPrinterSetting();
  ~SysPrinterSetting();

public:
  int GetCopies();
  int GetGamma();
  int GetBrightness();
  int GetMediaSize();
  int GetLandscape();
  int GetPageSize();
  int GetSonyGamma();

  void SetCopies(int copies);
  void SetGamma(int gamma);
  void SetBrightness(int brightness);
  void SetMediaSize(int media_size);
  void SetLandscape(int landscape_ang);
  void SetPageSize(int pagesize);
  void SetSonyGamma(int sonygamma);

  void DefaultFactory();
  void SyncFile();

private:
  IniFile* m_inifile;
};

#endif
