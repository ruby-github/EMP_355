#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include "Init.h"
#include "Def.h"
#include "sysMan/SysLog.h"
#include "VersionConfig.h"
#include "imageControl/Img2D.h"
#include "imageProc/Format2D.h"
#include "imageProc/ScanMode.h"
#include "imageProc/Replay.h"
#include "probe/ProbeMan.h"
#include "probe/ProbeSocket.h"
#include "imageProc/FreezeMode.h"
#include "probe/ExamItem.h"
#include <gtk/gtk.h>
#include "display/gui_global.h"
#include "display/gui_func.h"
#include "ViewMain.h"
#include "calcPeople/ViewReport.h"
#include "display/ImageArea.h"
#include "probe/ProbeSelect.h"
#include "imageProc/ImgProc2D.h"
#include <stdlib.h>
#include "patient/Database.h"
#include "measure/MeasureMan.h"
#include "calcPeople/MeaCalcFun.h"
#include "sysMan/UserSelect.h"
#include "periDevice/DCMMan.h"
#include "periDevice/PeripheralMan.h"
#include "sysMan/UpgradeMan.h"
#include "sysMan/ScreenSaver.h"
#include "sysMan/SysGeneralSetting.h"
#include "sysMan/SysCalculateSetting.h"
#include "sysMan/SysMeasurementSetting.h"
#include "sysMan/SysOptions.h"
#include "periDevice/Authorize.h"
#include "sysMan/ViewSystem.h"
#include "imageControl/FpgaGeneral.h"
#include "periDevice/DCMRegister.h"
#include "imageControl/UsbControl.h"

#include "utils/FakeXUtils.h"

pthread_t pidInitTable;
gboolean ReportInit = FALSE;
bool g_authorizationOn = false;
bool g_init = true;
bool g_calcPwStatus = false; // false: 未进行包络计算； true: 包络计算完成

//for test
using std::endl;

Init::Init() {
}

void Init::SystemInit(int argc, char* argv[]) {
  KeyboardOversee((void*)&g_keyInterface);
  KeyboardRequestTGC();

  // keyboard sound
  SysOptions *sysOpt = new SysOptions;
  int keySound = sysOpt->GetKeyboardSound();

  if (keySound) {
    KeyboardSound(false);
  } else {
    KeyboardSound(true);
  }

  int mouseSpeed = sysOpt->GetMouseSpeed();
  g_keyInterface.SetMouseSpeed(mouseSpeed);
  delete sysOpt;

  ///> GUI i18n
  setlocale(LC_ALL, "");
  bindtextdomain(PACKAGE, LOCALEDIR);
  bind_textdomain_codeset(PACKAGE, "UTF-8");
  textdomain(PACKAGE);

  SysGeneralSetting* sysGeneral = new SysGeneralSetting();
  int index_lang = sysGeneral->GetLanguage();
  int index_video_mode = sysGeneral->GetVideoMode();
  int tvout_format = sysGeneral->GetVideoFormat();

  delete sysGeneral;
  char str_cmd[256];

  if (ZH == index_lang) {
    setenv("LANG", "zh_CN.UTF-8", 1);
    setenv("LANGUAGE", "zh_CN:zh", 1);
    sprintf(str_cmd, "setxkbmap -layout %s", "us");
    _system_(str_cmd);
    //system("setxkbmap -layout us");
  } else if (RU == index_lang) {
    setenv("LANG", "ru_RU.UTF-8", 1);
    setenv("LANGUAGE", "ru_RU:ru", 1);
    //system("setxkbmap -layout ru");

    sprintf(str_cmd, "setxkbmap -layout %s", "ru");
    _system_(str_cmd);
  } else if (PL == index_lang) {
    setenv("LANG", "pl_PL.UTF-8", 1);
    setenv("LANGUAGE", "pl_PL:pl", 1);
    //system("setxkbmap -layout pl qwertz");
    sprintf(str_cmd, "setxkbmap -layout %s", "pl qwertz");
    _system_(str_cmd);
  } else if (ES == index_lang) {
    setenv("LANG", "es_ES.UTF-8", 1);
    setenv("LANGUAGE", "es_ES:es", 1);
    //system("setxkbmap -layout es");
    sprintf(str_cmd, "setxkbmap -layout %s", "es");
    _system_(str_cmd);
  } else if (FR == index_lang) {
    setenv("LANG", "fr_FR.UTF-8", 1);
    setenv("LANGUAGE", "fr_FR:fr", 1);
    //system("setxkbmap -layout fr");
    sprintf(str_cmd, "setxkbmap -layout %s", "fr");
    _system_(str_cmd);
  } else if (DE == index_lang) {
    setenv("LANG", "de_DE.UTF-8", 1);
    setenv("LANGUAGE", "de_DE:de", 1);
    //system("setxkbmap -layout de");
    sprintf(str_cmd, "setxkbmap -layout %s", "de");
    _system_(str_cmd);
  } else {
    setenv("LANG", "en_US.UTF-8", 1);
    setenv("LANGUAGE", "en_US:en", 1);
    //system("setxkbmap -layout us");
    sprintf(str_cmd, "setxkbmap -layout %s", "us");
    _system_(str_cmd);
  }

  SysCalculateSetting *sysCalc = new SysCalculateSetting();
  SysMeasurementSetting *sysMeasure = new SysMeasurementSetting();
  sysCalc->UpdateOBSetting();
  sysMeasure->UpdateTraceItemSetting();

  delete sysCalc;
  delete sysMeasure;

  // Database
  Database db;
  if (!db.CreateTable()) {
    return ;
  }

  db.OpenDB();

  UpgradeMan::GetInstance()->SetRebootState("1");
  CFpgaGeneral::GetInstance()->AD5805ControlInit();
}

void* ThreadReportTable(void* argv) {
  PRINTF("Enter thread to init report table data!\n");
  ViewReport::GetInstance()->InitTableData();
  PRINTF("Leave thread to init report table data!\n");
  ReportInit = TRUE;

  return NULL;
}

void Init::ParaInit() {
  // replay
  Replay::GetInstance()->Init();

  // Report Table
  if( pthread_create(&pidInitTable, NULL, ThreadReportTable, NULL) != 0) {
    PRINTF("Failed to create report table init thread!\n");
  }

  // Measure
  MeaCalcFun::InitCustomTable();
  MeasureMan::GetInstance()->ClearAllValue();

  //clear record
  g_patientInfo.ClearRecord();

  ///> watch Volume device
  PRINTF("begin add watch for peripheral device\n");
  PeripheralMan::GetInstance()->AddWatch();
  PeripheralMan::GetInstance()->SwitchPrinterDriver();

  ///> watch network cable
  PRINTF("begin add watch network cable\n");
  PeripheralMan::GetInstance()->AddWatchNetCable();

  ///> screen saver
  PRINTF("begin screen saver timer\n");
  SysGeneralSetting sgs;
  ScreenSaver::GetInstance()->SetPeriod(sgs.GetScreenProtect()*60);

  //creat dicom manual object
  CDCMMan::Create(DCMDB_DIR);
  CDCMRegister::Create(DCMRES_DIR);
}

void Init::ProbeCheck() {
  FreezeMode* ptrFreeze = FreezeMode::GetInstance();
  ptrFreeze->Freeze();

  // send 0 to fpga
  INT32U addr;
  INT32U temp;
  addr = 55;
  temp = 0x0;
  EzUsb::GetInstance()->WriteOneDataToFpga(addr, temp);
  ProbeSelect ps;
  ps.ProbeRead();

  // select user
  char path[256];
  sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
  IniFile ini(path);
  ExamItem exam;
  const char *userName;
  userName = exam.ReadDefaultUserSelect(&ini).c_str();

  if ((strcmp(userName, "System Default") != 0) &&
    (strcmp(userName, "Умолчан системы") != 0) &&
    (strcmp(userName, "系统默认") != 0) &&
    (strcmp(userName, "Domyślne Systemu") != 0) &&
    (strcmp(userName, "Par défaut du sys.") != 0) &&
    (strcmp(userName, "Systemvorgabe") != 0) &&
    (strcmp(userName, "Sistema por defecto") !=0)) {

    stringstream ss;
    ss << "userconfig/" << userName << ".ini";

    g_user_configure = ss.str();
  } else {
    g_user_configure = "ItemPara.ini";
  }

  string itemName;
  bool flag = false;
  ps.GetDefaultItemNameandFlag(itemName, flag);
  int indexSocket = 0; ///< start from 0
  ExamItem::EItem indexItem; ///< start form 0
  ps.GetDefaultValue(indexSocket, indexItem, flag);

  // probe init
  if(flag) {
    ps.UserItemOfProbeInit(indexSocket, indexItem, itemName.c_str());
  } else {
    ps.ProbeInit(indexSocket, indexItem);
  }

  // 退出刚开机状态
  g_init = false;

  IoCtrl io;
  io.BeginReadImage();
}

void Init::WriteLogHead() {
  class SysLog* sysLog = SysLog::Instance();
  class VersionConfig* version = VersionConfig::Instance();

  ///> get work-mode of software
  version->Init();

  if (version->GetProduct() == VersionConfig::TROLLEY) {
    (*sysLog) << "software work in trolley mode." << endl;
  } else if (version->GetProduct() == VersionConfig::PORTABLE) {
    (*sysLog) << "software work in portable mode." << endl;
  }

  ///> get realse configure version of software
  if (version->GetRealse() == VersionConfig::EMP_CHI) {
    (*sysLog) << "software is config to chinese language of emperor." << endl;
  } else if (version->GetRealse() == VersionConfig::EMP_EN) {
    (*sysLog) << "software is config to english language of emperor." << endl;
  }
}

void Init::ExitSystem() {
  //destory dicom manual object
  CDCMMan::Destroy();
  CDCMRegister::Destroy();

  // remove device info
  PeripheralMan::GetInstance()->RemoveWatch();

  // destroy global color
  free_init_colors();
}

void Init::CreatMainWindow() {
  ViewMain::GetInstance()->Create();
}
