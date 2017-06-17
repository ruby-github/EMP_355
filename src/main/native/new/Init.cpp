#include "Init.h"

#include "calcPeople/MeaCalcFun.h"
#include "calcPeople/ViewReport.h"
#include "imageControl/FpgaGeneral.h"
#include "imageControl/UsbControl.h"
#include "imageProc/FreezeMode.h"
#include "imageProc/Replay.h"
#include "keyboard/KeyValueOpr.h"
#include "measure/MeasureMan.h"
#include "patient/Database.h"
#include "patient/PatientInfo.h"
#include "periDevice/DCMMan.h"
#include "periDevice/DCMRegister.h"
#include "periDevice/IoCtrl.h"
#include "periDevice/PeripheralMan.h"
#include "probe/ExamItem.h"
#include "probe/ProbeSelect.h"
#include "sysMan/ScreenSaver.h"
#include "sysMan/SysCalculateSetting.h"
#include "sysMan/SysGeneralSetting.h"
#include "sysMan/SysLog.h"
#include "sysMan/SysMeasurementSetting.h"
#include "sysMan/SysOptions.h"
#include "sysMan/UpgradeMan.h"
#include "sysMan/ViewSystem.h"
#include "utils/FakeXUtils.h"
#include "utils/IniFile.h"
#include "utils/Utils.h"
#include "VersionConfig.h"
#include "ViewMain.h"

bool g_init = true;
bool g_reportInit = false;
bool g_authorizationOn = false;
bool g_calcPwStatus = false;  // false: 未进行包络计算, true: 包络计算完成
pthread_t g_pidInitTable;

void* ThreadReportTable(void* argv) {
  cout << "Enter thread to init report table data!" << endl;
  ViewReport::GetInstance()->InitTableData();
  cout << "Leave thread to init report table data!" << endl;

  g_reportInit = true;

  return NULL;
}

// ---------------------------------------------------------

Init::Init() {
}

Init::~Init() {
}

void Init::SystemInit(int argc, char* argv[]) {
  KeyboardOversee((void*)&g_keyInterface);
  KeyboardRequestTGC();

  // keyboard sound
  SysOptions sysOpt;
  int keySound = sysOpt.GetKeyboardSound();

  if (keySound) {
    KeyboardSound(false);
  } else {
    KeyboardSound(true);
  }

  int mouseSpeed = sysOpt.GetMouseSpeed();
  g_keyInterface.SetMouseSpeed(mouseSpeed);


  // GUI i18n
  setlocale(LC_ALL, "");
  bindtextdomain(PACKAGE, LOCALEDIR);
  bind_textdomain_codeset(PACKAGE, "UTF-8");
  textdomain(PACKAGE);

  SysGeneralSetting sysGeneral;
  int index_lang = sysGeneral.GetLanguage();
  int index_video_mode = sysGeneral.GetVideoMode();
  int tvout_format = sysGeneral.GetVideoFormat();

  if (ZH == index_lang) {
    setenv("LANG", "zh_CN.UTF-8", 1);
    setenv("LANGUAGE", "zh_CN:zh", 1);
    _system_("setxkbmap -layout us");
  } else if (RU == index_lang) {
    setenv("LANG", "ru_RU.UTF-8", 1);
    setenv("LANGUAGE", "ru_RU:ru", 1);
    _system_("setxkbmap -layout ru");
  } else if (PL == index_lang) {
    setenv("LANG", "pl_PL.UTF-8", 1);
    setenv("LANGUAGE", "pl_PL:pl", 1);
    _system_("setxkbmap -layout pl qwertz");
  } else if (ES == index_lang) {
    setenv("LANG", "es_ES.UTF-8", 1);
    setenv("LANGUAGE", "es_ES:es", 1);
    _system_("setxkbmap -layout es");
  } else if (FR == index_lang) {
    setenv("LANG", "fr_FR.UTF-8", 1);
    setenv("LANGUAGE", "fr_FR:fr", 1);
    _system_("setxkbmap -layout fr");
  } else if (DE == index_lang) {
    setenv("LANG", "de_DE.UTF-8", 1);
    setenv("LANGUAGE", "de_DE:de", 1);
    _system_("setxkbmap -layout de");
  } else {
    setenv("LANG", "en_US.UTF-8", 1);
    setenv("LANGUAGE", "en_US:en", 1);
    _system_("setxkbmap -layout us");
  }

  SysCalculateSetting sysCalc;
  sysCalc.UpdateOBSetting();

  SysMeasurementSetting sysMeasure;
  sysMeasure.UpdateTraceItemSetting();

  // Database
  Database db;
  if (!db.CreateTable()) {
    return;
  }

  db.OpenDB();

  UpgradeMan::GetInstance()->SetRebootState("1");
  CFpgaGeneral::GetInstance()->AD5805ControlInit();
}

void Init::ParaInit() {
  // replay
  Replay::GetInstance()->Init();

  // Report Table
  if(pthread_create(&g_pidInitTable, NULL, ThreadReportTable, NULL) != 0) {
    cerr << "failed to create report table init thread!" << endl;
  }

  // Measure
  MeaCalcFun::InitCustomTable();
  MeasureMan::GetInstance()->ClearAllValue();

  // clear record
  g_patientInfo.ClearRecord();

  // watch Volume device
  cout << "begin add watch for peripheral device" << endl;
  PeripheralMan::GetInstance()->AddWatch();
  PeripheralMan::GetInstance()->SwitchPrinterDriver();

  // watch network cable
  cout << "begin add watch network cable" << endl;
  PeripheralMan::GetInstance()->AddWatchNetCable();

  // screen saver
  cout << "begin screen saver timer" << endl;
  SysGeneralSetting sgs;
  ScreenSaver::GetInstance()->SetPeriod(sgs.GetScreenProtect() * 60);

  // creat dicom manual object
  CDCMMan::Create(DCMDB_DIR);
  CDCMRegister::Create(DCMRES_DIR);
}

void Init::ProbeCheck() {
  FreezeMode::GetInstance()->Freeze();

  // send 0 to fpga
  INT32U addr = 55;
  INT32U temp = 0x0;

  EzUsb::GetInstance()->WriteOneDataToFpga(addr, temp);

  ProbeSelect ps;
  ps.ProbeRead();

  // select user
  ExamItem exam;
  IniFile ini(string(CFG_RES_PATH) + string(STORE_DEFAULT_ITEM_PATH));
  string userName = exam.ReadDefaultUserSelect(&ini);

  if (userName != "System Default" && userName != "Умолчан системы" &&
    userName != "系统默认" && userName != "Domyślne Systemu" && userName != "Par défaut du sys." &&
    userName != "Systemvorgabe" && userName != "Sistema por defecto") {
    stringstream ss;
    ss << "userconfig/" << userName << ".ini";

    g_user_configure = ss.str();
  } else {
    g_user_configure = "ItemPara.ini";
  }

  string itemName;
  bool flag = false;
  ps.GetDefaultItemNameandFlag(itemName, flag);

  int indexSocket = 0;        // start from 0
  ExamItem::EItem indexItem;  // start form 0
  ps.GetDefaultValue(indexSocket, indexItem, flag);

  // probe init
  if(flag) {
    ps.UserItemOfProbeInit(indexSocket, indexItem, itemName);
  } else {
    ps.ProbeInit(indexSocket, indexItem);
  }

  // 退出刚开机状态
  g_init = false;

  IoCtrl io;
  io.BeginReadImage();
}

void Init::WriteLogHead() {
  SysLog* sysLog = SysLog::Instance();
  VersionConfig* version = VersionConfig::Instance();

  // get work-mode of software
  version->Init();

  if (version->GetProduct() == VersionConfig::TROLLEY) {
    (*sysLog) << "software work in trolley mode." << endl;
  } else if (version->GetProduct() == VersionConfig::PORTABLE) {
    (*sysLog) << "software work in portable mode." << endl;
  }

  // get realse configure version of software
  if (version->GetRealse() == VersionConfig::EMP_CHI) {
    (*sysLog) << "software is config to chinese language of emperor." << endl;
  } else if (version->GetRealse() == VersionConfig::EMP_EN) {
    (*sysLog) << "software is config to english language of emperor." << endl;
  }
}

void Init::CreatMainWindow() {
  ViewMain::GetInstance()->Create();
}

void Init::ExitSystem() {
  // destory dicom manual object
  CDCMMan::Destroy();
  CDCMRegister::Destroy();

  // remove device info
  PeripheralMan::GetInstance()->RemoveWatch();

  // destroy global color
  free_init_colors();
}
