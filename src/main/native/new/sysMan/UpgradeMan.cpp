#include "sysMan/UpgradeMan.h"

#include "utils/MessageDialog.h"

#include "Def.h"
#include "patient/FileMan.h"
#include "periDevice/PeripheralMan.h"
#include "sysMan/ViewSystem.h"

#define FILE_MAX_NUM 50

string upgradeManInfo[] = {
  N_("Sure to upgrade?"),
  N_("Upgrading, please wait..."),
  N_("Upgrade success,please reboot!"),
  N_("Upgrade failed: USB device not found!"),
  N_("Upgrade failed: The system has not been restarted  after last upgrade.\nPlease restart the system and try again!"),
  N_("Upgrade failed: Cannot mount USB device!"),
  N_("Upgrade failed: Cannot copy upgrade file!"),
  N_("Upgrade failed: Upgrade file not found!"),
  N_("Upgrade failed: Dest directory not found!"),
  N_("Upgrade failed!")
};

UpgradeMan* UpgradeMan::m_instance = NULL;

// ---------------------------------------------------------

UpgradeMan* UpgradeMan::GetInstance() {
  if (m_instance == NULL) {
    m_instance = new UpgradeMan();
  }

  return m_instance;
}

UpgradeMan::UpgradeMan() {
  NewIniFile();
}

UpgradeMan::~UpgradeMan() {
  if (m_inifile != NULL) {
    delete m_inifile;
  }

  m_inifile = NULL;

  if (m_instance != NULL) {
    delete m_instance;
  }

  m_instance = NULL;
}

string UpgradeMan::GetMachineType() {
  return m_inifile->ReadString("Upgrade", "MachineType");
}

string UpgradeMan::GetSoftVersion() {
  return m_inifile->ReadString("Upgrade", "SoftVersion");
}

string UpgradeMan::GetFpgaVersion() {
  return m_inifile->ReadString("Upgrade","FpgaVersion");
}

int UpgradeMan::GetRebootState() {
  return m_inifile->ReadInt("Upgrade", "Upgrade-Reboot");
}

int UpgradeMan::GetOrderState() {
  return m_inifile->ReadInt("Upgrade", "Upgrade-Order");
}

void UpgradeMan::SetRebootState(string val) {
  m_inifile->WriteString("Upgrade", "Upgrade-Reboot", val);
  m_inifile->SyncConfigFile();
}

void UpgradeMan::SetOrderState(string val) {
  m_inifile->WriteString("Upgrade", "Upgrade-Order", val);
  m_inifile->SyncConfigFile();
}

void UpgradeMan::Upgrade() {
  MessageDialog::GetInstance()->Create(
    GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()),
    MessageDialog::DLG_QUESTION, upgradeManInfo[QUESTION], signal_callback_ok_upgrade);
}

// ---------------------------------------------------------

void UpgradeMan::UpgradeOk() {
  MessageHintDialog::GetInstance()->Create(
    GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()),
    upgradeManInfo[UpgradeMan::PROGRESS]);

  g_timeout_add(500, signal_callback_timeout, this);
}

void UpgradeMan::TimeoutUpgrade() {
  int val = UpgradeMan::GetInstance()->SureToUpgrade();
  PeripheralMan::GetInstance()->UmountUsbStorage();

  MessageDialog::GetInstance()->Create(
    GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()),
    MessageDialog::DLG_INFO, upgradeManInfo[val], NULL);
}

void UpgradeMan::NewIniFile() {
  m_inifile = new IniFile(string(CFG_RES_PATH) + string(UPGRADE_INI_PATH));
}

int UpgradeMan::SureToUpgrade() {
  int reboot = GetRebootState();
  if(!reboot) {
    return E_REBOOT;
  }

  PeripheralMan* ptrPeripheralMan = PeripheralMan::GetInstance();

  if(!ptrPeripheralMan->CheckUsbStorageState()) {
    PRINTF("No UsbStorage Plug-in\n");
    return E_NO_USB;
  }

  if(!ptrPeripheralMan->MountUsbStorage()) {
    return E_MOUNT;
  }

  //check all of the dest file in the list
  PRINTF("\n%s: Begin to check the file in the list\n", __FUNCTION__);
  int i;
  string value;
  char* listPath = g_build_path(G_DIR_SEPARATOR_S, UPGRADE_SRC_PATH, UPGRADE_LIST, NULL);
  if (!g_file_test(listPath, G_FILE_TEST_EXISTS)) {
    g_free(listPath);

    return E_NO_SRC;
  }

  char* destAppPath;

  int order = GetOrderState();
  if(order==0) {
    destAppPath = g_build_path(G_DIR_SEPARATOR_S, UPGRADE_DEST_PATH, "1", NULL);
  } else if(order==1) {
    destAppPath = g_build_path(G_DIR_SEPARATOR_S, UPGRADE_DEST_PATH, "0", NULL);
  } else {
    g_free(listPath);
    return E_OTHER;
  }

  if(g_file_test(destAppPath, G_FILE_TEST_IS_DIR) < 0) {
    perror("Check dest dir error:");
    g_free(listPath);
    g_free(destAppPath);

    return E_NO_DEST;
  }

  FileMan fm;
  char* src;
  char* dest;
  vector<string> vecSrc;
  vector<string> vecDest;
  vecSrc.clear();
  vecDest.clear();
  IniFile ini(listPath);

  char tmp[5];

  for(i=1; i<=FILE_MAX_NUM; i++) {
    sprintf(tmp, "%d", i);
    value = ini.ReadString("Upgrade-List", tmp);
    PRINTF("Read Upgrade List : %s\n", value.c_str());

    if (strcmp(value.c_str(), "") != 0) {
      if(strcmp(value.c_str(), APP_NAME)==0) {
        dest = g_build_filename(destAppPath, value.c_str(), NULL);
      } else {
        dest = g_build_filename(UPGRADE_DEST_PATH, value.c_str(), NULL);
      }

      PRINTF("dest file = %s\n", dest);

      src = g_build_filename(UPGRADE_SRC_PATH, value.c_str(), NULL);
      PRINTF("src = %s\n", src);

      if (g_file_test(src, G_FILE_TEST_EXISTS)) {
        if (g_file_test(src, G_FILE_TEST_IS_DIR)) {
          if(!fm.CheckDir(dest)) {
            g_free(src);
            g_free(dest);
            g_free(destAppPath);

            return E_NO_DEST;
          }
        } else if(g_file_test(src, G_FILE_TEST_IS_REGULAR)) {
          vecSrc.push_back(src);
          vecDest.push_back(dest);
          g_free(src);
          g_free(dest);
        }
      } else {
        perror("Check source file error:");
        g_free(src);
        g_free(dest);
        g_free(destAppPath);

        return E_NO_SRC;
      }
    } else {
      break;
    }
  }

  g_free(destAppPath);
  g_free(listPath);

  // copy dest to src
  PRINTF("\n%s: Begin to copy file to dest\n", __FUNCTION__);
  int retval = UpgradeFile(vecSrc, vecDest);

  if(retval != SUCCESS) {
    return retval;
  } else {
    return SUCCESS;
  }
}

int UpgradeMan::UpgradeFile(vector<string> src, vector<string> dest) {
  vector<string>::iterator  iteSrc, iteDest;
  PRINTF("%s: source vector size = %d, dest vector size = %d\n", __FUNCTION__, src.size(), dest.size());

  if(src.size() != dest.size() || src.size() == 0 || dest.size() == 0) {
    return E_OTHER;
  }

  int order = GetOrderState();
  FileMan fm;

  for(iteSrc=src.begin(), iteDest=dest.begin(); iteSrc<src.end(), iteDest<dest.end(); iteSrc++, iteDest++) {
    PRINTF("%s: Copy %s To %s\n", __FUNCTION__, (*iteSrc).c_str(), (*iteDest).c_str());

    if(fm.CopyFile((*iteSrc).c_str(), (*iteDest).c_str()) < 0) {
      return E_COPY;
    }

    gchar* basename = g_path_get_basename((*iteDest).c_str());
    gchar* name2 = g_path_get_basename(UPGRADE_INI_PATH);

    if(strcmp(basename, APP_NAME)==0 || strcmp(basename, name2)==0) {
      if (strcmp(basename, name2)==0) {
        // delete old ini file
        if (m_inifile != NULL) {
          delete m_inifile;
          m_inifile = NULL;
        }

        NewIniFile(); // reload new ini file
      }

      char tmp[5];
      if(order==0) {
        sprintf(tmp, "1");
      } else if(order==1) {
        sprintf(tmp, "0");
      } else {
      }

      SetOrderState(tmp);
      SetRebootState("0");
    }

    g_free(basename);
  }

  return SUCCESS;
}

#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "display/gui_global.h"
#include "display/gui_func.h"
