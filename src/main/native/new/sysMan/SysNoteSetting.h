#ifndef __SYS_NOTE_SETTING_H__
#define __SYS_NOTE_SETTING_H__

#include "utils/IniFile.h"

class SysNoteSetting {
public:
  SysNoteSetting();
  ~SysNoteSetting();

public:
  int GetFontSize();
  int GetBodyMarkSize();
  int GetFontColor();
  int GetBodyMarkColor();

  void SetFontSize(int fontSize);
  void SetBodyMarkSize(int bodyMark);
  void SetFontColor(int fontColor);
  void SetBodyMarkColor(int bodyMarkColor);

  void DefaultFactory();
  void SyncFile();

private:
  IniFile* m_inifile;
};

#endif
