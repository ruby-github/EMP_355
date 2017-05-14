#ifndef __SYS_OPTIONS_H__
#define __SYS_OPTIONS_H__

#include "utils/IniFile.h"

class SysOptions {
public:
  SysOptions();
  ~SysOptions();

public:
  int GetBiopsyLine();
  int GetCenterLine();
  int GetTI();
  int GetKeyboardSound();
  int GetImageFormat();
  int GetDisplayFormatM();
  int GetDisplayFormatPW();
  int GetImageMedia();
  int GetImageAutoName();
  int GetCineFormat();
  int GetCineMedia();
  int GetCineAutoName();
  int GetMouseSpeed();

  void SetBiopsyLine(int biopsyLine);
  void SetCenterLine(int centerLine);
  void SetTI(int TI);
  void SetKeyboardSound(int keyboardSound);
  void SetImageFormat(int imageFormat);
  void SetDisplayFormatM(int dispalyFormat);
  void SetDisplayFormatPW(int dispalyFormat);
  void SetImageMedia(int imageMedia);
  void SetImageAutoName(int imageAutoName);
  void SetCineFormat(int cineFormat);
  void SetCineMedia(int cineMedia);
  void SetCineAutoName(int cineAutoName);
  void SetMouseSpeed(int mouseSpeed);

  void DefaultFactory();
  void SyncFile();

private:
  IniFile* m_inifile;
};

#endif
