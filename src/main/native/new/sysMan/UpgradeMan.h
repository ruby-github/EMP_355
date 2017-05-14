#ifndef __UPGRADE_MAN_H__
#define __UPGRADE_MAN_H__

#include "utils/IniFile.h"

class UpgradeMan {
public:
  static UpgradeMan* GetInstance();

public:
  ~UpgradeMan();

  string GetMachineType();
  string GetSoftVersion();
  string GetFpgaVersion();

  int GetRebootState();
  int GetOrderState();

  void SetRebootState(string val);
  void SetOrderState(string al);

  void Upgrade();

private:
  // signal

  static gboolean signal_callback_ok_upgrade(gpointer data) {
    UpgradeMan::GetInstance()->UpgradeOk();

    return TRUE;
  }

  static gboolean signal_callback_timeout(gpointer data) {
    UpgradeMan* upgrademan = (UpgradeMan*)data;

    if (upgrademan != NULL) {
      upgrademan->TimeoutUpgrade();
    }

    return FALSE;
  }

  // signal

  void UpgradeOk();
  void TimeoutUpgrade();

private:
  UpgradeMan();

  void NewIniFile();

  int SureToUpgrade();
  int UpgradeFile(vector<string> src, vector<string> dest);

  enum {QUESTION, PROGRESS, SUCCESS, E_NO_USB, E_REBOOT, E_MOUNT, E_COPY, E_NO_SRC, E_NO_DEST, E_OTHER};

private:
  static UpgradeMan* m_instance;

private:
  IniFile* m_inifile;
};

#endif
