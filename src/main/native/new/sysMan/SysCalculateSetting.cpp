#include "sysMan/SysCalculateSetting.h"

#include "Def.h"
#include "calcPeople/MeaCalcFun.h"
#include "calcPeople/MeasureDef.h"

struct OBSetting {
  int cer;
  int hl;
  int bpd;
  int fl;
  int crl;
  int gs;
  int ac;
  int hc;
  int tad;
  int apad;
  int thd;
  int ofd;
  int efw;
};

OBSetting obSetting;

// ---------------------------------------------------------

SysCalculateSetting::SysCalculateSetting() {
  m_inifile = new IniFile(string(CFG_RES_PATH) + string(SYS_SETTING_FILE));
}

SysCalculateSetting::~SysCalculateSetting() {
  if (m_inifile != NULL) {
    delete m_inifile;
  }

  m_inifile = NULL;
}

int SysCalculateSetting::GetBSAMethod() {
  return m_inifile->ReadInt("CalculateSetting", "BSA");
}

int SysCalculateSetting::GetCerMethod() {
  return m_inifile->ReadInt("CalculateSetting", "Cer");
}

int SysCalculateSetting::GetHlMethod() {
  return m_inifile->ReadInt("CalculateSetting", "Hl");
}

int SysCalculateSetting::GetBpdMethod() {
  return m_inifile->ReadInt("CalculateSetting", "Bpd");
}

int SysCalculateSetting::GetFlMethod() {
  return m_inifile->ReadInt("CalculateSetting", "Fl");
}

int SysCalculateSetting::GetCrlMethod() {
  return m_inifile->ReadInt("CalculateSetting", "Crl");
}

int SysCalculateSetting::GetGsMethod() {
  return m_inifile->ReadInt("CalculateSetting", "Gs");
}
int SysCalculateSetting::GetAcMethod() {
  return m_inifile->ReadInt("CalculateSetting", "Ac");
}

int SysCalculateSetting::GetHcMethod() {
  return m_inifile->ReadInt("CalculateSetting", "Hc");
}

int SysCalculateSetting::GetTadMethod() {
  return m_inifile->ReadInt("CalculateSetting", "Tad");
}

int SysCalculateSetting::GetApadMethod() {
  return m_inifile->ReadInt("CalculateSetting", "Apad");
}

int SysCalculateSetting::GetThdMethod() {
  return m_inifile->ReadInt("CalculateSetting", "Thd");
}

int SysCalculateSetting::GetOfdMethod() {
  return m_inifile->ReadInt("CalculateSetting", "Ofd");
}

int SysCalculateSetting::GetEfwMethod() {
  return m_inifile->ReadInt("CalculateSetting", "Efw");
}

int SysCalculateSetting::GetCurEfwMethod() {
  return obSetting.efw + EFW_START;
}

int SysCalculateSetting::GetGWMethod(int item) {
  int method = 0;

  switch(item) {
  case OB_CER:
    method = obSetting.cer;
    method += CER_START - 1;
    break;
  case OB_HL:
    method = obSetting.hl;
    method += HL_START - 1;
    break;
  case OB_BPD:
    method = obSetting.bpd;
    method += BPD_START - 1;
    break;
  case OB_FL:
    method = obSetting.fl;
    method += FL_START - 1;
    break;
  case OB_CRL:
    method = obSetting.crl;
    method += CRL_START - 1;
    break;
  case OB_GS:
    method = obSetting.gs;
    method += GS_START - 1;
    break;
  case OB_AC:
    method = obSetting.ac;
    method += AC_START - 1;
    break;
  case OB_HC:
    method = obSetting.hc;
    method += HC_START - 1;
    break;
  case OB_TAD:
    method = obSetting.tad;
    method += TAD_START - 1;
    break;
  case OB_APAD:
    method = obSetting.apad;
    method += APAD_START - 1;
    break;
  case OB_THD:
    method = obSetting.thd;
    method += THD_START - 1;
    break;
  case OB_OFD:
    method = obSetting.ofd;
    method += OFD_START - 1;
    break;
  case OB_OOD:
    // method = GetOodMethod();
    // method += OOD_START - 1;
    method = OOD_START - 1;
    break;
  case OB_TIBIA:
    // method = GetTibiaMethod();
    // method += TIBIA_START - 1;
    method = TIBIA_START - 1;
    break;
  case OB_TTD:
    // method = GetTtdMethod();
    // method += TTD_START - 1;
    method = TTD_START - 1;
    break;
  case OB_ULNA:
    // method = GetUlnaMethod();
    // method += ULNA_START - 1;
    method = ULNA_START - 1;
    break;
  case OB_FTA:
    method = FTA_START - 1;
    break;
  default:
    break;
  }

  return(method);
}

void SysCalculateSetting::SetBSAMethod(int BSA) {
  m_inifile->WriteInt("CalculateSetting", "BSA", BSA);
}

// cer 0-goldstein 1-custom
void SysCalculateSetting::SetCerMethod(int cer) {
  m_inifile->WriteInt("CalculateSetting", "Cer", cer);
}

// hl 0-jeanty 1-custom
void SysCalculateSetting::SetHlMethod(int hl) {
  m_inifile->WriteInt("CalculateSetting", "Hl", hl);
}

// bpd 0-hadlock 1-merz 2-lasser 3-rempen 4-tokyo 5-custom
void SysCalculateSetting::SetBpdMethod(int bpd) {
  m_inifile->WriteInt("CalculateSetting", "Bpd", bpd);
}

// fl 0-hadlock 1-merz 2-jeanty 3-tokyo 4-custom
void SysCalculateSetting::SetFlMethod(int fl) {
  m_inifile->WriteInt("CalculateSetting", "Fl", fl);
}

// crl 0-hadlock 2-robinson 3-hansmenn 4-lasser 5-tokyo 6-custom
void SysCalculateSetting::SetCrlMethod(int crl) {
  m_inifile->WriteInt("CalculateSetting", "Crl", crl);
}

// gs 0-hellman 1-rempen 2-tokyo 3-custom
void SysCalculateSetting::SetGsMethod(int gs) {
  m_inifile->WriteInt("CalculateSetting", "Gs", gs);
}

// ac 0-hadlock 1-merz 2-lasser 3-tokyo 4-custom
void SysCalculateSetting::SetAcMethod(int ac) {
  m_inifile->WriteInt("CalculateSetting", "Ac", ac);
}

// hc 0-hadlock 1-merz 2-lasser 3-custom
void SysCalculateSetting::SetHcMethod(int hc) {
  m_inifile->WriteInt("CalculateSetting", "Hc", hc);
}

// tad 0-merz 1-custom
void SysCalculateSetting::SetTadMethod(int tad) {
  m_inifile->WriteInt("CalculateSetting", "Tad", tad);
}

// apad 0-merz 1-custom
void SysCalculateSetting::SetApadMethod(int apad) {
  m_inifile->WriteInt("CalculateSetting", "Apad", apad);
}

// thd 0-hansmenn 1-custom
void SysCalculateSetting::SetThdMethod(int thd) {
  m_inifile->WriteInt("CalculateSetting", "Thd", thd);
}

// ofd 0-korean 1-custom
void SysCalculateSetting::SetOfdMethod(int ofd) {
  m_inifile->WriteInt("CalculateSetting", "Ofd", ofd);
}

// efw 0-hadlock1 2-hadlock2 3-hadlock3 4-hadlock4 5-shepard 6hansmenn 7-tokyo
void SysCalculateSetting::SetEfwMethod(int efw) {
  m_inifile->WriteInt("CalculateSetting", "Efw", efw);
}

void SysCalculateSetting::DefaultFactory() {
  int BSA=0;
  int Cer=0;
  int Hl=0;
  int Bpd=0;
  int Fl=0;
  int Crl=0;
  int Gs=0;
  int Ac=0;
  int Hc=0;
  int Tad=0;
  int Apad=0;
  int Thd=0;
  int Ofd=0;
  int Efw=0;

  m_inifile->WriteInt("CalculateSetting", "BSA", BSA);
  m_inifile->WriteInt("CalculateSetting", "Cer", Cer);
  m_inifile->WriteInt("CalculateSetting", "Hl", Hl);
  m_inifile->WriteInt("CalculateSetting", "Bpd", Bpd);
  m_inifile->WriteInt("CalculateSetting", "Fl", Fl);
  m_inifile->WriteInt("CalculateSetting", "Crl", Crl);
  m_inifile->WriteInt("CalculateSetting", "Gs", Gs);
  m_inifile->WriteInt("CalculateSetting", "Ac", Ac);
  m_inifile->WriteInt("CalculateSetting", "Hc", Hc);
  m_inifile->WriteInt("CalculateSetting", "Tad", Tad);
  m_inifile->WriteInt("CalculateSetting", "Apad", Apad);
  m_inifile->WriteInt("CalculateSetting", "Thd", Thd);
  m_inifile->WriteInt("CalculateSetting", "Ofd", Ofd);
  m_inifile->WriteInt("CalculateSetting", "Efw", Efw);
}

void SysCalculateSetting::UpdateOBSetting() {
  obSetting.cer = GetCerMethod();
  obSetting.hl = GetHlMethod();
  obSetting.bpd = GetBpdMethod();
  obSetting.fl = GetFlMethod();
  obSetting.crl = GetCrlMethod();
  obSetting.gs = GetGsMethod();
  obSetting.ac = GetAcMethod();
  obSetting.hc = GetHcMethod();
  obSetting.tad = GetTadMethod();
  obSetting.apad = GetApadMethod();
  obSetting.thd = GetThdMethod();
  obSetting.ofd = GetOfdMethod();
  obSetting.efw = GetEfwMethod();
}

void SysCalculateSetting::SyncFile() {
  m_inifile->SyncConfigFile();
}
