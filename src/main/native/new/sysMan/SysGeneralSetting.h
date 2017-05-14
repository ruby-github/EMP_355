#ifndef __SYSGENERALSETTING_H__
#define __SYSGENERALSETTING_H__

#include "utils/IniFile.h"

class SysGeneralSetting {
public:
  SysGeneralSetting();
  ~SysGeneralSetting();

public:
  string GetHospital();
  int GetLanguage();
  int GetDateFormat();
  int GetVideoMode();
  int GetConnectorType();
  int GetVideoFormat();
  int GetPrinterMethod();
  int GetPrinter();
  int GetScreenProtect();

  int GetPrintViewMode();
  int GetAutoGeneratedID();
  int GetFID();
  int GetDirection();
  int GetScaleUnit();
  int GetTraceType();
  int GetSmooth();
  int GetThreshold();

  void SetHospital(string hospital);
  void SetLanguage(int language);
  void SetDateFormat(int format);
  void SetVideoMode(int videoMode);
  void SetConnectorType(int connectorType);
  void SetVideoFormat(int format);
  void SetPrinterMethod(int method);
  void SetPrinter(int printer);
  void SetScreenProtect(int screenProtect);
  void SetPrintViewMode(int viewMode);
  void SetAutoGeneratedID(int autoGen);
  void SetDirection(int direction);
  void SetScaleUnit(int scaleunit);
  void SetTraceType(int tracetype);
  void SetSmooth(int smooth);
  void SetThreshold(int threshold);

  void DefaultFactory();
  void TVOutDefaultFactory();
  void SyncFile();

  #ifdef TRANSDUCER
    int GetPressAjust();
    void SetPressAdjust(double PressAjust);
  #endif

private:
  IniFile* m_inifile;
};

#endif
