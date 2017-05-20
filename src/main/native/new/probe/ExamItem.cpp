#include "probe/ExamItem.h"

#include <sstream>

#include "Def.h"
#include "imageProc/ModeStatus.h"
#include "imageProc/ScanMode.h"
#include "patient/FileMan.h"
#include "probe/ViewProbe.h"
#include "sysMan/ViewSystem.h"

using namespace std;

// 0: AdultAbdo         - 成人腹部
// 1: AdultLiver        - 成人肝胆脾
// 2: KidAbdo           - 小儿腹部
// 3: AdultCardio       - 成人心脏
// 4: KidCardio         - 小儿心脏
// 5: MammaryGlands     - 乳腺
// 6: Thyroid           - 甲状腺
// 7: EyeBall           - 眼球
// 8: Testicle          - 浅表器官
// 9: Gyn               - 妇科
// 10: EarlyPreg        - 早孕
// 11: MiddleLaterPreg  - 中晚孕
// 12: FetusCardio      - 胎儿心脏
// 13: KidneyUreter     - 双肾输尿管
// 14: BladderProstate  - 膀胱前列腺
// 15: Carotid          - 颈动脉
// 16: Jugular          - 颈静脉
// 17: PeripheryArtery  - 外周动脉
// 18: PeripheryVein    - 外周静脉
// 19: HipJoint         - 髋关节
// 20: Meniscus         - 半月板
// 21: JointCavity      - 关节腔
// 22: Spine            - 脊柱
// 23: TCD              - 经颅多普勒
// 24: User1            - 用户自定义1
// 25: User2            - 用户自定义2
// 26: User3            - 用户自定义3
// 27: User4            - 用户自定义4

const string ExamItem::ITEM_LIB[NUM_ITEM] = {
  "AdultAbdo",      "AdultLiver",       "KidAbdo",          "AdultCardio",    "KidCardio",
  "MammaryGlands",  "Thyroid",          "EyeBall",          "Testicle",       "Gyn",
  "EarlyPreg",      "MiddleLaterPreg",  "FetusCardio",      "KidneyUreter",   "BladderProstate",
  "Carotid",        "Jugular",          "PeripheryArtery",  "PeripheryVein",  "HipJoint",
  "Meniscus",       "JointCavity",      "Spine",            "TCD",            "User1",
  "User2",          "User3",            "User4"
};

const string KEY_COMMON = "Common";

// ---------------------------------------------------------

ExamItem::ExamItem() {
  m_probeIndex = 0;

  m_itemIndex = ABDO_ADULT;
  InitItemPara(&m_paraItem);
  InitItemPara(&m_paraOptimize);

  int i;
  for (i = 0; i < NUM_PROBE; i ++) {
    m_vecItemIndex[i].clear();
  }

  InitItemOfProbe();
  m_genFirstItem = "Carotid";
}

ExamItem::~ExamItem() {
}

vector<string> ExamItem::GetUserGroup() {
  IniFile ini(string(CFG_RES_PATH) + string(EXAM_ITEM_FILE));

  return ini.GetGroupName();
}

vector<string> ExamItem::GetCurrentUserGroup() {
  string username = ViewSystem::GetInstance()->GetUserName();

  if (username == "System Default") {
    username = "SystemDefault";
  }

  stringstream ss;
  ss << CFG_RES_PATH << EXAM_ITEM_FILE << username << ".ini";

  vector<string> vec;

  FILE* fp = fopen(ss.str().c_str(), "r");

  if (fp != NULL) {
    fclose(fp);

    IniFile ini(ss.str());
    vec = ini.GetGroupName();
  }

  return vec;
}

vector<string> ExamItem::GetDefaultUserGroup() {
  stringstream ss;
  ss << CFG_RES_PATH << STORE_DEFAULT_ITEM_PATH;

  IniFile ini_default(ss.str());
  string username = ReadDefaultUserSelect(&ini_default);

  if (username == "System Default") {
    username = "SystemDefault";
  }

  ss.str("");
  ss << CFG_RES_PATH << EXAM_ITEM_FILE << username << ".ini";

  vector<string> vec;

  FILE* fp = fopen(ss.str().c_str(), "r");

  if (fp != NULL) {
    fclose(fp);

    IniFile ini(ss.str());
    vec = ini.GetGroupName();
  }

  return vec;
}

void ExamItem::GetUserItem(const string group, string& userselect, string& probelist_value,
  string& useritem_value, string& department_value, string& genFirstItem) {
  string username = ViewSystem::GetInstance()->GetUserName();

  if (username == "System Default") {
    username = "SystemDefault";
  }

  stringstream ss;
  ss << CFG_RES_PATH << EXAM_ITEM_FILE << username << ".ini";

  IniFile ini(ss.str());

  userselect = ini.ReadString(group, "UserSelect");
  probelist_value = ini.ReadString(group, "ProbeType");
  useritem_value = ini.ReadString(group, "ExamItem");
  department_value = ini.ReadString(group, "Department");
  genFirstItem = ini.ReadString(group, "GenFirstItem");
}

void ExamItem::GetDefaultUserItem(const string group, string& userselect, string& probelist_value,
  string& useritem_value, string& department_value, string& genFirstItem) {
  stringstream ss;
  ss << CFG_RES_PATH << STORE_DEFAULT_ITEM_PATH;

  IniFile ini_default(ss.str());
  string username = ReadDefaultUserSelect(&ini_default);

  if (username == "System Default") {
    username = "SystemDefault";
  }

  ss.str("");
  ss << CFG_RES_PATH << EXAM_ITEM_FILE << username << ".ini";

  FILE* fp = fopen(ss.str().c_str(), "r");

  if (fp != NULL) {
    fclose(fp);

    IniFile ini(ss.str());

    userselect = ini.ReadString(group, "UserSelect");
    probelist_value = ini.ReadString(group, "ProbeType");
    useritem_value = ini.ReadString(group, "ExamItem");
    department_value = ini.ReadString(group, "Department");
    genFirstItem = ini.ReadString(group, "GenFirstItem");
  } else {
    userselect = "";
    probelist_value = "";
    useritem_value = "";
    department_value = "";
    genFirstItem = "";
  }
}

void ExamItem::DeleteUserItem(const string group) {
  stringstream ss;
  ss << CFG_RES_PATH << STORE_DEFAULT_ITEM_PATH;

  IniFile ini_default(ss.str());
  string username = ReadDefaultUserSelect(&ini_default);

  if (username == "System Default") {
    username = "SystemDefault";
  }

  ss.str("");
  ss << CFG_RES_PATH << EXAM_ITEM_FILE << username << ".ini";

  FILE* fp = fopen(ss.str().c_str(), "r");

  if (fp != NULL) {
    fclose(fp);

    IniFile ini(ss.str());

    ini.RemoveGroup(group);
    ini.SyncConfigFile();
  }
}

string ExamItem::TransItemName(const string str) {
  if (str == _("Adult Abdomen")) {
    return "AdultAbdo";
  } else if (str == _("Adult Liver")) {
    return "AdultLiver";
  } else if (str == _("Kid Abdomen")) {
    return "KidAbdo";
  } else if (str == _("Adult Cardio")) {
    return "AdultCardio";
  } else if (str == _("Kid Cardio")) {
    return "KidCardio";
  } else if (str == _("Mammary Glands")) {
    return "MammaryGlands";
  } else if (str == _("Thyroid")) {
    return "Thyroid";
  } else if (str == _("Eye Ball")) {
    return "EyeBall";
  } else if (str == _("Testicle")) {
    return "Testicle";
  } else if (str == _("Gynecology")) {
    return "Gyn";
  } else if (str == _("Early Pregnancy")) {
    return "EarlyPreg";
  } else if (str == _("Middle-late Pregnancy")) {
    return "MiddleLaterPreg";
  } else if (str == _("Fetal Cardio")) {
    return "FetusCardio";
  } else if (str == _("Kidney Ureter")) {
    return "KidneyUreter";
  } else if (str == _("Bladder Prostate")) {
    return "BladderProstate";
  } else if (str == _("Carotid")) {
    return "Carotid";
  } else if (str == _("Jugular")) {
    return "Jugular";
  } else if (str == _("Periphery Artery")) {
    return "PeripheryArtery";
  } else if (str == _("Periphery Vein")) {
    return "PeripheryVein";
  } else if (str == _("Hip Joint")) {
    return "HipJoint";
  } else if (str == _("Meniscus")) {
    return "Meniscus";
  } else if (str == _("Joint Cavity")) {
    return "JointCavity";
  } else if (str == _("Spine")) {
    return "Spine";
  } else if (str == _("TCD")) {
    return "TCD";
  } else {
    return str;
  }
}

string ExamItem::TransItemNameEng(const string str) {
  if (str == "Adult Abdomen") {
    return "AdultAbdo";
  } else if (str == "Adult Liver") {
    return "AdultLiver";
  } else if (str == "Kid Abdomen") {
    return "KidAbdo";
  } else if (str == "Adult Cardio") {
    return "AdultCardio";
  } else if (str == "Kid Cardio") {
    return "KidCardio";
  } else if (str == "Mammary Glands") {
    return "MammaryGlands";
  } else if (str == "Thyroid") {
    return "Thyroid";
  } else if (str == "Eye Ball") {
    return "EyeBall";
  } else if (str == "Testicle") {
    return "Testicle";
  } else if (str == "Gynecology") {
    return "Gyn";
  } else if (str == "Early Pregnancy") {
    return "EarlyPreg";
  } else if (str == "Middle-late Pregnancy") {
    return "MiddleLaterPreg";
  } else if (str == "Fetal Cardio") {
    return "FetusCardio";
  } else if (str == "Kidney Ureter") {
    return "KidneyUreter";
  } else if (str == "Bladder Prostate") {
    return "BladderProstate";
  } else if (str == "Carotid") {
    return "Carotid";
  } else if (str == "Jugular") {
    return "Jugular";
  } else if (str == "Periphery Artery") {
    return "PeripheryArtery";
  } else if (str == "Periphery Vein") {
    return "PeripheryVein";
  } else if (str == "Hip Joint") {
    return "HipJoint";
  } else if (str == "Meniscus") {
    return "Meniscus";
  } else if (str == "Joint Cavity") {
    return "JointCavity";
  } else if (str == "Spine") {
    return "Spine";
  } else if (str == "TCD") {
    return "TCD";
  } else {
    return str;
  }
}

string ExamItem::TransUserSelectForEng(const string name) {
  if (name == _("System Default")) {
    return "System Default";
  } else {
    return name;
  }
}

vector<ExamItem::EItem> ExamItem::GetItemListOfProbe(const string probeModel) {
  int probeIndex = GetProbeIndex(probeModel);

  return m_vecItemIndex[probeIndex];
}

vector<string> ExamItem::GetUserItemListOfProbe(const string probeModel) {
  m_vecUserItemName.clear();
  GetUserItemInfo(probeModel);

  return m_vecUserItemName;
}

string ExamItem::GetUserItemInfo(const string probeModel) {
  IniFile ini(string(CFG_RES_PATH) + string(STORE_DEFAULT_ITEM_PATH));
  string username = ReadDefaultUserSelect(&ini);

  vector<string> useritemgroup = GetDefaultUserGroup();
  string str_user_item_name = ViewProbe::GetInstance()->GetItemNameUserDef();

  for (int i= 0; i < useritemgroup.size(); i++) {
    string userselect;
    string probelist;
    string useritem;
    string department;
    string genFirstItem;

    GetDefaultUserItem(useritemgroup[i], userselect, probelist, useritem, department, genFirstItem);

    ExamPara exampara;

    exampara.dept_name = department;
    exampara.name = useritem;
    exampara.index = ExamItem::USERNAME;

    if(username == userselect) {
      if (probeModel == probelist) {
        m_vecUserItemName.push_back(exampara.name);

        if(str_user_item_name == useritem) {
          m_genFirstItem = genFirstItem;

          return genFirstItem;
        }
      }
    }
  }

  return "Adult Abdomen";
}

string ExamItem::GetInitUserItemInfo(const string probeModel, const string inituseritem) {
  IniFile ini(string(CFG_RES_PATH) + string(STORE_DEFAULT_ITEM_PATH));
  string username = ReadDefaultUserSelect(&ini);

  vector<string> useritemgroup = GetDefaultUserGroup();

  for (int i= 0 ; i < useritemgroup.size(); i++) {
    string userselect;
    string probelist;
    string useritem;
    string department;
    string genFirstItem;

    GetDefaultUserItem(useritemgroup[i], userselect, probelist, useritem, department, genFirstItem);

    ExamPara exampara;
    exampara.dept_name=department;
    exampara.name = useritem;
    exampara.index = ExamItem::USERNAME;

    if (username == userselect) {
      if (probeModel == probelist) {
        m_vecUserItemName.push_back(exampara.name);

        if(inituseritem == useritem) {
          m_genFirstItem = genFirstItem;

          return genFirstItem;
        }
      }
    }
  }

  return "Adult Abdomen";
}

ExamItem::EItem ExamItem::GetDefaultItem(const string probeModel) {
  m_probeIndex = GetProbeIndex(probeModel);
  m_itemIndex = m_vecItemIndex[m_probeIndex][0];

  return m_itemIndex;
}

string ExamItem::GetFirstGenItem() {
  return m_genFirstItem;
}

ExamItem::ParaItem ExamItem::GetCurrentItemPara() {
  return m_paraItem;
}

ExamItem::ParaItem ExamItem::GetImgOptimize(const string probeModel) {
  IniFile ini(string(CFG_RES_PATH) + string(OPTIMIZE_FILE));

  int probeIndex = GetProbeIndex(probeModel);
  ReadConfigCommon(&m_paraOptimize, PROBE_LIST[probeIndex], &ini);
  ReadConfigOther(&m_paraOptimize, PROBE_LIST[probeIndex], &ini);

  return m_paraOptimize;
}

void ExamItem::GenerateDefaultExamItem() {
  int i;
  int j;

  InitItemPara(&m_paraItem);
  InitItemOfProbe();

  IniFile ini(string(CFG_RES_PATH) + string(DEFAULT_EXAM_FILE));

  for (int i = 0; i < NUM_ITEM; i ++) {
    WriteConfigCommon(&m_paraItem, KEY_COMMON + "-" + ITEM_LIB[i], &ini);
  }

  // write different para in each item and each probe
  // 一个探头对应一组检查项目，见m_vecItemIndex[]， 最多MAX_ITEM（16）个。
  for (int i = 0; i < NUM_PROBE; i ++) {
    for (int j = 0; j < m_vecItemIndex[i].size(); j ++) {
      int itemIndex = m_vecItemIndex[i][j];
      WriteConfigOther(&m_paraItem, PROBE_LIST[i] + "-" + ITEM_LIB[itemIndex], &ini);
    }
  }
}

void ExamItem::GenerateDefaultImgOptimize() {
  IniFile ini(string(CFG_RES_PATH) + string(DEFAULT_OPTIMIZE_FILE));
  InitItemPara(&m_paraOptimize);

  for (int i = 0; i < NUM_PROBE; i ++) {
    WriteConfigCommon(&m_paraOptimize, PROBE_LIST[i], &ini);
    WriteConfigOther(&m_paraOptimize, PROBE_LIST[i], &ini);
  }
}

void ExamItem::SetUserItemOfProbe(const string probeModel, EItem itemIndex, const string item) {
  m_probeIndex = GetProbeIndex(probeModel);
  m_itemIndex = itemIndex;

  stringstream ss;

  if (strcmp(user_configure, "ItemPara.ini") == 0) {
    ss << CFG_RES_PATH << EXAM_FILE;
  } else {
    ss << CFG_RES_PATH << EXAM_FILE_DIR << user_configure;
  }

  IniFile ini(ss.str());
  ReadConfigCommon(&m_paraItem, KEY_COMMON + "-" + item, &ini);
  ReadConfigOther(&m_paraItem, PROBE_LIST[m_probeIndex] + "-" + item, &ini);
}

void ExamItem::SetItemOfProbe(const string probeModel, EItem itemIndex) {
  m_probeIndex = GetProbeIndex(probeModel);
  m_itemIndex = itemIndex;

  stringstream ss;

  if (strcmp(user_configure, "ItemPara.ini") == 0) {
    ss << CFG_RES_PATH << EXAM_FILE;
  } else {
    ss << CFG_RES_PATH << EXAM_FILE_DIR << user_configure;
  }

  IniFile ini(ss.str());
  ReadConfigCommon(&m_paraItem, KEY_COMMON + "-" + ITEM_LIB[m_itemIndex], &ini);
  ReadConfigOther(&m_paraItem, PROBE_LIST[m_probeIndex] + "-" + ITEM_LIB[m_itemIndex], &ini);
}

void ExamItem::RemoveUserFromFile(const string model, const string user, IniFile* ptrIni) {
  if(model.empty()|| user.empty()) {
    perror("input is null\n");

    return;
  }

  ptrIni->RemoveGroup(model + "-" + user);
  ptrIni->SyncConfigFile();
}

void ExamItem::ReadDefaultExamItem(int probeIndex, int itemIndex, ParaItem* paraItem, const string itemName) {
  IniFile ini(string(CFG_RES_PATH) + string(DEFAULT_EXAM_FILE));

  if(itemIndex == ExamItem::USERNAME) {
    ReadConfigCommon(paraItem, KEY_COMMON + "-" + itemName, &ini);
    ReadConfigOther(paraItem, PROBE_LIST[probeIndex] + "-" + itemName, &ini);
  } else {
    ReadConfigCommon(paraItem, KEY_COMMON + "-" + ITEM_LIB[itemIndex], &ini);
    ReadConfigOther(paraItem, PROBE_LIST[probeIndex] + "-" + ITEM_LIB[itemIndex], &ini);
  }
}

void ExamItem::DeleteItemParaFile(int probeIndex, const string old_string, const string str) {
  FileMan fm;

  stringstream ss;

  string name = ViewSystem::GetInstance()->GetUserName();
  if (name == "System Default" && name == "Умолчан системы" &&
    name == "系统默认" && name == "Domyślne Systemu" && name == "Par défaut du sys." &&
    name == "Systemvorgabe" &&  name == "Sistema por defecto") {
    ss << CFG_RES_PATH << EXAM_FILE_DIR << user_configure;
  } else {
    ss << CFG_RES_PATH << EXAM_FILE;

    string path_other = string(CFG_RES_PATH) + string(EXAM_FILE_OTHER);

    FILE *fd;
    if( (fd = fopen(path_other.c_str(), "r")) == NULL) {
      PRINTF("open default itempara file error\n");
      fm.CopyFile(ss.str().c_str(), path_other.c_str());
    }

    fclose(fd);

    IniFile ini_other(path_other);
    DeleteNewExamItem(str, KEY_COMMON + "-" + old_string, &ini_other);
    DeleteNewExamItem(str, PROBE_LIST[probeIndex] + "-" + old_string, &ini_other);
  }

  IniFile ini(ss.str());

  DeleteNewExamItem(str, KEY_COMMON + "-" + old_string, &ini);
  DeleteNewExamItem(str, PROBE_LIST[probeIndex] + "-" + old_string, &ini);
}

int ExamItem::ReadDefaultProbeDefaultItem(IniFile* ptrIni) {
  return ptrIni->ReadInt("Item", "DefaultItem");
}

void ExamItem::WriteDefaultProbeItem(IniFile* ptrIni, int item) {
  ptrIni->WriteInt("Item", "DefaultItem", item);
  ptrIni->SyncConfigFile();
}

void ExamItem::WriteSelectedProbeItem(const string probeModel, IniFile* ptrIni, int item) {
  ptrIni->WriteInt(probeModel, "SelectProbeItem", item);
  ptrIni->SyncConfigFile();
}

void ExamItem::ReadExamItemPara(int probeIndex, int itemIndex, ParaItem* paraItem, const string itemName) {
  stringstream ss;

  string name = ViewSystem::GetInstance()->GetUserName();
  if (name == "System Default" && name == "Умолчан системы" &&
    name == "系统默认" && name == "Domyślne Systemu" && name == "Par défaut du sys." &&
    name == "Systemvorgabe" &&  name == "Sistema por defecto") {
    ss << CFG_RES_PATH << EXAM_FILE_DIR << user_configure;
  } else {
    ss << CFG_RES_PATH << EXAM_FILE;
  }

  IniFile ini(ss.str());

  if (probeIndex >= 0) {
    if(itemIndex == ExamItem::USERNAME) {
      ReadConfigCommon(paraItem, KEY_COMMON + "-" + itemName, &ini);
      ReadConfigOther(paraItem, PROBE_LIST[probeIndex] + "-" + itemName, &ini);
    } else {
      ReadConfigCommon(paraItem, KEY_COMMON + "-" + ITEM_LIB[itemIndex], &ini);
      ReadConfigOther(paraItem, PROBE_LIST[probeIndex] + "-" + ITEM_LIB[itemIndex], &ini);
    }
  }
}

void ExamItem::WriteExamItemPara(int probeIndex, int itemIndex, ParaItem* paraItem, const string itemName) {
  stringstream ss;

  string name = ViewSystem::GetInstance()->GetUserName();
  FileMan fm;

  if (name == "System Default" &&  name == "Умолчан системы" &&
    name == "系统默认" && name == "Domyślne Systemu" && name == "Par défaut du sys." &&
    name == "Systemvorgabe" && name == "Sistema por defecto") {
    ss << CFG_RES_PATH << EXAM_FILE_DIR << "userconfig/" << name << ".ini";
  } else {
    ss << CFG_RES_PATH << EXAM_FILE;

    string path_other = string(CFG_RES_PATH) + string(EXAM_FILE_OTHER);

    FILE *fd;
    if( (fd = fopen(path_other.c_str(), "r")) == NULL) {
      PRINTF("open default itempara file error\n");
      fm.CopyFile(ss.str().c_str(), path_other.c_str());
    }

    fclose(fd);

    IniFile ini_other(path_other);
    if(itemIndex == ExamItem::USERNAME) {
      WriteConfigCommon(paraItem, KEY_COMMON + "-" + itemName, &ini_other);
      WriteConfigOther(paraItem, PROBE_LIST[probeIndex] + "-" + itemName, &ini_other);
    } else {
      WriteConfigCommon(paraItem, KEY_COMMON + "-" + ITEM_LIB[itemIndex], &ini_other);
      WriteConfigOther(paraItem, PROBE_LIST[probeIndex] + "-" + ITEM_LIB[itemIndex], &ini_other);
    }
  }

  IniFile ini(ss.str());
  if(itemIndex == ExamItem::USERNAME) {
    WriteConfigCommon(paraItem, KEY_COMMON + "-" + itemName, &ini);
    WriteConfigOther(paraItem, PROBE_LIST[probeIndex] + "-" + itemName, &ini);
  } else {
    WriteConfigCommon(paraItem, KEY_COMMON + "-" + ITEM_LIB[itemIndex], &ini);
    WriteConfigOther(paraItem, PROBE_LIST[probeIndex] + "-" + ITEM_LIB[itemIndex], &ini);
  }
}

void ExamItem::WriteDefinedItemPara(int probeIndex, const string new_string, const string str, const string str_index) {
  stringstream ss;

  string name = ViewSystem::GetInstance()->GetUserName();
  if (name == "System Default" && name == "Умолчан системы" &&
    name == "系统默认" && name == "Domyślne Systemu" && name == "Par défaut du sys." &&
    name == "Systemvorgabe" &&  name == "Sistema por defecto") {
    ss << CFG_RES_PATH << EXAM_FILE_DIR << user_configure;
  } else {
    ss << CFG_RES_PATH << EXAM_FILE;
  }

  IniFile ini(ss.str());
  WriteDefinedExamItemPara(str, PROBE_LIST[probeIndex] + "-" + new_string, &ini, PROBE_LIST[probeIndex], new_string, str_index);
}

void ExamItem::WriteDefaultDefinedItemPara(int probeIndex, const string new_string, const string str, const string str_index) {
  IniFile ini(string(CFG_RES_PATH) + string(DEFAULT_EXAM_FILE));

  WriteDefaultDefinedExamItemPara(str, PROBE_LIST[probeIndex] + "-" + new_string, &ini, PROBE_LIST[probeIndex], new_string, str_index);
}

void ExamItem::WriteDefinedExamItemPara(const string department, string section, IniFile* ptrIni, string probelist, const string new_string, const string str_index) {
  string str_name = TransItemNameEng(str_index);

  IniFile Ini(string(CFG_RES_PATH) + string(DEFAULT_EXAM_FILE));
  ParaItem paradefinedItem;

  ReadConfigCommon(&paradefinedItem, KEY_COMMON + "-" + str_name, &Ini);
  ReadConfigOther(&paradefinedItem, probelist + "-" + str_name, &Ini);
  paradefinedItem.d2.rotate = Ini.ReadInt((probelist + "-" + str_name).c_str(), "D2-Rotate");

  stringstream ss;

  string name = ViewSystem::GetInstance()->GetUserName();
  if (name == "System Default" && name == "Умолчан системы" &&
    name == "系统默认" && name == "Domyślne Systemu" && name == "Par défaut du sys." &&
    name == "Systemvorgabe" &&  name == "Sistema por defecto") {
    ss << CFG_RES_PATH << EXAM_FILE_DIR << user_configure;
  } else {
    ss << CFG_RES_PATH << EXAM_FILE;
  }

  IniFile ini(ss.str());
  WriteConfigCommon(&paradefinedItem, KEY_COMMON + "-" + new_string, &ini);
  WriteConfigOther(&paradefinedItem, probelist + "-" + new_string, &ini);
  ini.WriteInt(probelist + "-" + new_string, "Spectrum-DynamicRange", paradefinedItem.spectrum.dynamicRange);
  ini.WriteInt(probelist + "-" + new_string, "Spectrum-SampleVolume", paradefinedItem.spectrum.SV);
  ini.WriteInt(probelist + "-" + new_string, "D2-Rotate", paradefinedItem.d2.rotate);
  ini.SyncConfigFile();
}

void ExamItem::WriteDefaultDefinedExamItemPara(const string department, string section, IniFile* ptrIni, string probelist, const string new_string, const string str_index) {
  string str_name = TransItemNameEng(str_index);

  ParaItem paraItem;
  IniFile Ini(string(CFG_RES_PATH) + string(EXAM_FILE));

  ReadConfigCommon(&paraItem, KEY_COMMON + "-" + str_name, &Ini);
  ReadConfigOther(&paraItem, probelist + "-" + str_name, &Ini);
  paraItem.d2.rotate = Ini.ReadInt((probelist + "-" + str_name).c_str(), "D2-Rotate");

  WriteConfigCommon(&paraItem, KEY_COMMON + "-" + new_string, ptrIni);
  WriteConfigOther(&paraItem, probelist + "-" + new_string, ptrIni);
  Ini.WriteInt(probelist + "-" + new_string, "Spectrum-DynamicRange", paraItem.spectrum.dynamicRange);
  Ini.WriteInt(probelist + "-" + new_string, "Spectrum-SampleVolume", paraItem.spectrum.SV);
  Ini.WriteInt(probelist + "-" + new_string, "D2-Rotate", paraItem.d2.rotate);
  Ini.SyncConfigFile();
}

void ExamItem::WriteNewItemFile(int probeIndex, const string new_string, const string str, const string str_index) {
  string username = ViewSystem::GetInstance()->GetUserName();
  if(username == _("System Default")) {
    username == "SystemDefault";
  }

  stringstream ss;
  ss << CFG_RES_PATH << EXAM_ITEM_FILE << username << ".ini";

  int fd;
  fd = open(ss.str().c_str(), O_RDWR |O_CREAT, 0666);
  close(fd);

  IniFile ini(ss.str());
  WriteNewExamItem(str, PROBE_LIST[probeIndex] + "-" + new_string, &ini, PROBE_LIST[probeIndex], new_string, str_index);

  string name = ViewSystem::GetInstance()->GetUserName();
  if (name == "System Default" && name == "Умолчан системы" &&
    name == "系统默认" && name == "Domyślne Systemu" && name == "Par défaut du sys." &&
    name == "Systemvorgabe" &&  name == "Sistema por defecto") {

    ss.str("");
    ss << CFG_RES_PATH << EXAM_FILE_DIR << user_configure;
  } else {
    ss.str("");
    ss << CFG_RES_PATH << EXAM_FILE;

    IniFile ini_other(string(CFG_RES_PATH) + string(EXAM_FILE_OTHER));
    WriteNewExamItem(str, PROBE_LIST[probeIndex] + "-" + new_string, &ini_other, PROBE_LIST[probeIndex], new_string, str_index);
  }

  IniFile ini_new(ss.str());

  WriteNewExamItem(str, PROBE_LIST[probeIndex] + "-" + new_string, &ini_new, PROBE_LIST[probeIndex], new_string, str_index);
}

void ExamItem::DeleteNewItemFile(int probeIndex, const string old_string, const string str) {
  string username = ViewSystem::GetInstance()->GetUserName();
  if(username == _("System Default")) {
    username == "SystemDefault";
  }

  stringstream ss;
  ss << CFG_RES_PATH << EXAM_ITEM_FILE << username << ".ini";

  IniFile ini(ss.str());
  DeleteNewExamItem(str, PROBE_LIST[probeIndex] + "-" + old_string, &ini);
}

void ExamItem::WriteNewItemToCommentFile(int probeIndex, const string new_string, const string str) {
  string username = ViewSystem::GetInstance()->GetUserName();

  stringstream ss;

  if(username == _("System Default")) {
    ss << CFG_RES_PATH << COMMENT_FILE;
  } else {
    ss << CFG_RES_PATH << COMMENT_PATH << username << ".ini";
  }

  IniFile ini(ss.str());

  ini.WriteInt(PROBE_LIST[probeIndex] + "-" + new_string, "Number", 0);
  ini.WriteString(PROBE_LIST[probeIndex] + "-" + new_string, "Department", str);
  ini.SyncConfigFile();
}

void ExamItem::WriteNewItemToCalcFile(int probeIndex, const string new_string, const string str) {
  string username = ViewSystem::GetInstance()->GetUserName();

  stringstream ss;

  if(username == _("System Default")) {
    ss << CFG_RES_PATH << CALC_FILE;
  } else {
    ss << CFG_RES_PATH << CALC_PATH << username << ".ini";
  }

  IniFile ini(ss.str());

  ini.WriteInt(new_string, "Number", 0);
  ini.SyncConfigFile();
}

void ExamItem::WriteNewItemToMeasureFile(int probeIndex, const string new_string, const string str) {
  string username = ViewSystem::GetInstance()->GetUserName();

  stringstream ss;

  if(username == _("System Default")) {
    ss << CFG_RES_PATH << MEASURE_FILE;
  } else {
    ss << CFG_RES_PATH << MEASURE_PATH << username << ".ini";
  }

  IniFile ini(ss.str());

  ini.WriteInt(new_string, "Number", 0);
  ini.SyncConfigFile();
}

void ExamItem::DeleteNewItemForCommentFile(int probeIndex, const string old_string, const string str) {
  string username = ViewSystem::GetInstance()->GetUserName();

  stringstream ss;

  if(username == _("System Default")) {
    ss << CFG_RES_PATH << COMMENT_FILE;
  } else {
    ss << CFG_RES_PATH << COMMENT_PATH << username << ".ini";
  }

  IniFile ini(ss.str());

  ini.RemoveGroup(PROBE_LIST[probeIndex] + "-" + old_string);
  ini.SyncConfigFile();
}

void ExamItem::DeleteNewItemForCalcFile(int probeIndex, const string old_string, const string str) {
  string username = ViewSystem::GetInstance()->GetUserName();

  stringstream ss;

  if(username == _("System Default")) {
    ss << CFG_RES_PATH << CALC_FILE;
  } else {
    ss << CFG_RES_PATH << CALC_PATH << username << ".ini";
  }

  IniFile ini(ss.str());

  ini.RemoveGroup(old_string);
  ini.SyncConfigFile();
}

void ExamItem::DeleteNewItemForMeasureFile(int probeIndex, const string old_string, const string str) {
  string username = ViewSystem::GetInstance()->GetUserName();

  stringstream ss;

  if(username == _("System Default")) {
    ss << CFG_RES_PATH << MEASURE_FILE;
  } else {
    ss << CFG_RES_PATH << MEASURE_PATH << username << ".ini";
  }

  IniFile ini(ss.str());

  ini.RemoveGroup(old_string);
  ini.SyncConfigFile();
}

void ExamItem::WriteUserItemFlag(IniFile* ptrIni, bool flag) {
  ptrIni->WriteBool("UserItemFlag", "UserFlag", flag);
  ptrIni->SyncConfigFile();
}

bool ExamItem::ReadUserItemFlag(IniFile* ptrIni) {
  return ptrIni->ReadBool("UserItemFlag", "UserFlag");
}

void ExamItem::WriteDefaultUserSelect(IniFile* ptrIni, const string username) {
  ptrIni->WriteString("UserSelect", "DefaultUser", username);
  ptrIni->SyncConfigFile();
}

string ExamItem::ReadDefaultUserSelect(IniFile* ptrIni) {
  return ptrIni->ReadString("UserSelect", "DefaultUser");
}

void ExamItem::WriteDefaultProbe(const string probeModel, IniFile* ptrIni) {
  ptrIni->WriteString("ProbeModel", "ProbeModel", probeModel);
  ptrIni->SyncConfigFile();
}

string ExamItem::ReadDefaultProbe(IniFile* ptrIni) {
  return ptrIni->ReadString("ProbeModel", "ProbeModel");
}

void ExamItem::WriteDefaultUserIndex(IniFile* ptrIni, int index) {
  ptrIni->WriteInt("UserIndex", "DefaultUserIndex", index);
  ptrIni->SyncConfigFile();
}

int ExamItem::ReadDefaultUserIndex(IniFile* ptrIni) {
  return ptrIni->ReadInt("UserIndex", "DefaultUserIndex");
}

void ExamItem::WriteDefaultProbeItemName(IniFile* ptrIni, const string itemName) {
  ptrIni->WriteString("UserItemName", "ItemName", itemName);
  ptrIni->SyncConfigFile();
}

string ExamItem::ReadDefaultProbeDefaultItemName(IniFile* ptrIni) {
  return ptrIni->ReadString("UserItemName", "ItemName");
}

void ExamItem::WriteProjectPara(ExamItem::ProjectDefaultParaItem* proParaItem, const string model, const string user, IniFile* ptrIni) {
  if (model.empty() || user.empty()) {
    return;
  }

  stringstream ss;
  string group_name = model + "-" + user;

  if((ScanMode::EScanMode)proParaItem->mode == ScanMode::D2) {
    ss << "D2-" << proParaItem->pro2d.freq << "-";
    string key_name = ss.str();

    ptrIni->WriteInt(group_name, key_name + "FilterSection", proParaItem->pro2d.FilterSection);
    ptrIni->WriteDoubleList(group_name, key_name + "BandPassW1", (double*)proParaItem->pro2d.BandPassW1, ExamItem::BAND_PASS_SIZE_D2);
    ptrIni->WriteDoubleList(group_name, key_name + "BandPassW2", (double*)proParaItem->pro2d.BandPassW2, ExamItem::BAND_PASS_SIZE_D2);
    ptrIni->WriteInt(group_name, key_name + "BandPassWindowFunc", proParaItem->pro2d.BandPassWindowFunc);
    ptrIni->WriteDoubleList(group_name, key_name + "LowPassW", (double*)proParaItem->pro2d.LowPassW, ExamItem::LOW_PASS_SIZE_D2);
    ptrIni->WriteInt(group_name, key_name + "LowPassWindowFunc", proParaItem->pro2d.LowPassWindowFunc);
  } else if((ScanMode::EScanMode)proParaItem->mode == ScanMode::CFM) {
    ss << "Color-" << proParaItem->procolor.dopfreq << "-";
    string key_name = ss.str();

    ptrIni->WriteInt(group_name, key_name + "WallFilterSection", proParaItem->procolor.WallFilterSection);
    ptrIni->WriteDoubleList(group_name, key_name + "WallFilterW", (double*)proParaItem->procolor.WallFilterW, ExamItem::WALL_FILTER_SIZE_COLOR);
  }

  ptrIni->SyncConfigFile();
}

vector<ExamItem::ProjectDefaultParaItem> ExamItem::ReadProjectPara(const string model, const string user, IniFile* ptrIni) {
  vector<ExamItem::ProjectDefaultParaItem> projectPara;

  if(model.empty() || user.empty()) {
    return projectPara;
  }

  stringstream ss;
  int index = 0;

  ExamItem::ProjectDefaultParaItem para;

  for(int i = 0; i < NUM_PROBE; i++) {
    if (model == PROBE_LIST[i]) {
      index = i;
      int freq_2d_sum = ProbeSocket::FREQ2D_SUM[index];
      int freq_doppler_sum = ProbeSocket::FREQ_DOPPLER_SUM[index];
      string group_name = PROBE_LIST[index] + "-" + "Default";

      for (int j = 0; j < ProbeSocket::FREQ2D_SUM[index]; j++) {
        ss << "D2-" << ProbeSocket::FREQ2D[index][j].emit << "-";
        string key_name = ss.str();

        para.pro2d.FilterSection = ptrIni->ReadInt(group_name, key_name + "FilterSection");
        int size = ptrIni->ReadDoubleList(group_name, key_name + "BandPassW1").size();

        if (size >= ExamItem::BAND_PASS_SIZE_D2) {
          size = ExamItem::BAND_PASS_SIZE_D2;

          for(int k = 0; k < size; k++) {
            para.pro2d.BandPassW1[k] = ptrIni->ReadDoubleList(group_name, key_name + "BandPassW1")[k];
          }
        } else {
          for(int k = 0; k < size; k++) {
            para.pro2d.BandPassW1[k] = ptrIni->ReadDoubleList(group_name, key_name + "BandPassW1")[k];
          }

          for(int k = size; k < ExamItem::BAND_PASS_SIZE_D2; k++) {
            para.pro2d.BandPassW1[k] = 0;
          }
        }

        size = ptrIni->ReadDoubleList(group_name, key_name + "BandPassW2").size();

        if(size >= ExamItem::BAND_PASS_SIZE_D2) {
          size = ExamItem::BAND_PASS_SIZE_D2;

          for(int k = 0; k < size; k++) {
            para.pro2d.BandPassW2[k] = ptrIni->ReadDoubleList(group_name, key_name + "BandPassW2")[k];
          }
        } else {
          for(int k = 0; k < size; k++) {
            para.pro2d.BandPassW2[k] = ptrIni->ReadDoubleList(group_name, key_name + "BandPassW2")[k];
          }

          for(int k = size; k < ExamItem::BAND_PASS_SIZE_D2; k++) {
            para.pro2d.BandPassW2[k] = 0;
          }
        }

        para.pro2d.BandPassWindowFunc = ptrIni->ReadInt(group_name, key_name + "BandPassWindowFunc");
        size = ptrIni->ReadDoubleList(group_name, key_name + "LowPassW").size();

        if(size >= ExamItem::LOW_PASS_SIZE_D2) {
          size = ExamItem::LOW_PASS_SIZE_D2;

          for(int k = 0; k < size; k++) {
            para.pro2d.LowPassW[k] = ptrIni->ReadDoubleList(group_name, key_name + "LowPassW")[k];
          }
        } else {
          for(int k = 0; k < size; k++) {
            para.pro2d.LowPassW[k] = ptrIni->ReadDoubleList(group_name, key_name + "LowPassW")[k];
          }

          for(int k = size; k < ExamItem::BAND_PASS_SIZE_D2; k++) {
            para.pro2d.LowPassW[k] = 0;
          }
        }

        para.pro2d.LowPassWindowFunc = ptrIni->ReadInt(group_name, key_name + "LowPassWindowFunc");

        // cfm
        if (j > ProbeSocket::FREQ_DOPPLER_SUM[index]-1) {
          projectPara.push_back(para);

          continue;
        }

        ss << "Color-" << ProbeSocket::FREQ_DOPPLER[index][j] << "-";
        key_name = ss.str();

        para.procolor.WallFilterSection = ptrIni->ReadInt(group_name, key_name + "WallFilterSection");
        size = ptrIni->ReadDoubleList(group_name, key_name + "WallFilterW").size();

        if(size >= ExamItem::WALL_FILTER_SIZE_COLOR) {
          size = ExamItem::WALL_FILTER_SIZE_COLOR;

          for(int k = 0; k < size; k++) {
            para.procolor.WallFilterW[k] = ptrIni->ReadDoubleList(group_name, key_name)[k];
          }
        } else {
          for(int k = 0; k < size; k++) {
            para.procolor.WallFilterW[j] = ptrIni->ReadDoubleList(group_name, key_name)[k];
          }

          for(int k = size; k < ExamItem::WALL_FILTER_SIZE_COLOR; k++) {
            para.procolor.WallFilterW[k] = 0;
          }
        }
      }
    }
  }

  return projectPara;
}

void ExamItem::WriteProDebugParaItem(ProjectDebugParaItem* debugPara, const string model, const string user, IniFile* ptrIni) {
  if(model.empty() || user.empty()) {
    return;
  }

  string group_name = model + "-" + user;

  ptrIni->WriteInt(group_name, "depth", debugPara->depth);
  ptrIni->WriteInt(group_name, "mode", debugPara->mode);
  ptrIni->WriteInt(group_name, "freq", debugPara->freq);
  WriteProjectPara(&debugPara->defaultPara, model, user, ptrIni);
}

void ExamItem::ReadProDebugParaItem(ProjectDebugParaItem* debugPara, const string model, const string user, IniFile* ptrIni) {
  stringstream ss;
  string group_name = model + "-" + user;
  vector<double> value;

  debugPara->depth = ptrIni->ReadInt(group_name, "depth");
  debugPara->mode = ptrIni->ReadInt(group_name, "mode");
  debugPara->freq = ptrIni->ReadInt(group_name, "freq");

  if ((ScanMode::EScanMode)debugPara->mode == ScanMode::D2) {
    ss << "D2-" << debugPara->freq << "-";
    string key_name = ss.str();

    debugPara->defaultPara.pro2d.FilterSection = ptrIni->ReadInt(group_name, key_name + "FilterSection");

    // bandPassW1
    value = ptrIni->ReadDoubleList(group_name, key_name + "BandPassW1");
    for(int i = 0; i < BAND_PASS_SIZE_D2; i++) {
      debugPara->defaultPara.pro2d.BandPassW1[i] = value[i];
    }

    // bandPassW2
    value = ptrIni->ReadDoubleList(group_name, key_name + "BandPassW2");
    for(int i = 0; i < BAND_PASS_SIZE_D2; i++) {
      debugPara->defaultPara.pro2d.BandPassW2[i] = value[i];
    }

    // bandPassWindowFunc
    debugPara->defaultPara.pro2d.BandPassWindowFunc = ptrIni->ReadInt(group_name, key_name + "BandPassWindowFunc");

    // lowPassW
    value = ptrIni->ReadDoubleList(group_name, key_name + "LowPassW");
    for(int i = 0; i < BAND_PASS_SIZE_D2; i++) {
      debugPara->defaultPara.pro2d.LowPassW[i] = value[i];
    }

    // LowPassWindowFunc
    debugPara->defaultPara.pro2d.LowPassWindowFunc = ptrIni->ReadInt(group_name, key_name + "LowPassWindowFunc");
  } else if((ScanMode::EScanMode)debugPara->mode == ScanMode::CFM) {
    ss << "Color-" << debugPara->freq << "-";
    string key_name = ss.str();

    debugPara->defaultPara.procolor.WallFilterSection = ptrIni->ReadInt(group_name, key_name + "WallFilterSection");

    value = ptrIni->ReadDoubleList(group_name, key_name + "WallFilterW");
    for(int i = 0; i < WALL_FILTER_SIZE_COLOR; i++) {
      debugPara->defaultPara.procolor.WallFilterW[i] = value[i];
    }
  }
}

// ---------------------------------------------------------

void ExamItem::InitItemPara(ParaItem* paraItem) {
  // common
  paraItem->common.MBP = 0;
  paraItem->common.powerIndex = 9;
  paraItem->common.textType = 0;
  paraItem->common.bodymarkType = 0;

  // 2D
  paraItem->d2.freqIndex = 2;
  paraItem->d2.imgScale = 1;
  paraItem->d2.gain2D = 100;
  paraItem->d2.focSum = 1;
  paraItem->d2.focPosIndex = 10;
  paraItem->d2.scanAngle = 0;
  paraItem->d2.dynamicRange = 0;
  paraItem->d2.lineDensity = 1;
  paraItem->d2.steerIndex = 0;
  paraItem->d2.AGC = 0;
  paraItem->d2.edgeEnhance = 0;
  paraItem->d2.harmonic = FALSE;
  paraItem->d2.TSI = 0;
  paraItem->d2.chroma = 0;
  paraItem->d2.leftRight = FALSE;
  paraItem->d2.upDown = FALSE;
  paraItem->d2.polarity = FALSE;
  paraItem->d2.rotate = 0;
  paraItem->d2.frameAver = 3;
  paraItem->d2.lineAver = 1;
  paraItem->d2.smooth = 1;
  paraItem->d2.noiseReject = 0;
  paraItem->d2.imgEhn = 2;
  paraItem->d2.gainM = 100;

  // spectrum
  paraItem->spectrum.freq = 0; // index of color freq

  if(ModeStatus::IsCWImgMode()) {
    paraItem->spectrum.gain = 50;
  } else if(ModeStatus::IsPWImgMode()) {
    paraItem->spectrum.gain = 60;
  }

  paraItem->spectrum.dynamicRange = 60;
  paraItem->spectrum.PRF = 1;
  paraItem->spectrum.wallFilter = 0;
  paraItem->spectrum.invert = FALSE;
  paraItem->spectrum.timeSmooth = 0;
  paraItem->spectrum.correctAngle = 0;
  paraItem->spectrum.SV = 5;

  // color
  paraItem->color.gain = 70;
  paraItem->color.PRFIndex = 1;
  paraItem->color.wallFilter = 0;
  paraItem->color.lineDensity = 0;
  paraItem->color.sensitive = 0;
  paraItem->color.turb = 0;
  paraItem->color.invert = FALSE;
  paraItem->color.reject = 0;
  paraItem->color.smooth = 0;
  paraItem->color.persist = 0;
}

void ExamItem::InitItemOfProbe() {
  // probe 0 35C505 13
  m_vecItemIndex[0].push_back(ABDO_ADULT);
  m_vecItemIndex[0].push_back(ABDO_LIVER);
  m_vecItemIndex[0].push_back(ABDO_KID);
  m_vecItemIndex[0].push_back(GYN);
  m_vecItemIndex[0].push_back(EARLY_PREG);
  m_vecItemIndex[0].push_back(LATER_PREG);
  m_vecItemIndex[0].push_back(CAR_FETUS);
  m_vecItemIndex[0].push_back(KIDNEY);
  m_vecItemIndex[0].push_back(BLADDER);
  m_vecItemIndex[0].push_back(HIP_JOINT);
  m_vecItemIndex[0].push_back(MENISCUS);
  m_vecItemIndex[0].push_back(JOINT_CAVITY);
  m_vecItemIndex[0].push_back(SPINE);

  // probe 1 75L40K 8
  m_vecItemIndex[1].push_back(GLANDS);
  m_vecItemIndex[1].push_back(THYROID);
  m_vecItemIndex[1].push_back(EYE);
  m_vecItemIndex[1].push_back(SMALL_PART);
  m_vecItemIndex[1].push_back(HIP_JOINT);
  m_vecItemIndex[1].push_back(MENISCUS);
  m_vecItemIndex[1].push_back(JOINT_CAVITY);
  m_vecItemIndex[1].push_back(SPINE);

  m_vecItemIndex[1].push_back(CAROTID);
  m_vecItemIndex[1].push_back(JUGULAR);
  m_vecItemIndex[1].push_back(PERI_ARTERY);
  m_vecItemIndex[1].push_back(PERI_VEIN);

  // 75L40J
  m_vecItemIndex[2].push_back(GLANDS);
  m_vecItemIndex[2].push_back(THYROID);
  m_vecItemIndex[2].push_back(EYE);
  m_vecItemIndex[2].push_back(SMALL_PART);
  m_vecItemIndex[2].push_back(HIP_JOINT);
  m_vecItemIndex[2].push_back(MENISCUS);
  m_vecItemIndex[2].push_back(JOINT_CAVITY);
  m_vecItemIndex[2].push_back(SPINE);
  m_vecItemIndex[2].push_back(CAROTID);
  m_vecItemIndex[2].push_back(JUGULAR);
  m_vecItemIndex[2].push_back(PERI_ARTERY);
  m_vecItemIndex[2].push_back(PERI_VEIN);
  // 70L60J
  m_vecItemIndex[3].push_back(GLANDS);
  m_vecItemIndex[3].push_back(THYROID);
  m_vecItemIndex[3].push_back(EYE);
  m_vecItemIndex[3].push_back(SMALL_PART);
  m_vecItemIndex[3].push_back(HIP_JOINT);
  m_vecItemIndex[3].push_back(MENISCUS);
  m_vecItemIndex[3].push_back(JOINT_CAVITY);
  m_vecItemIndex[3].push_back(SPINE);
  m_vecItemIndex[3].push_back(CAROTID);
  m_vecItemIndex[3].push_back(JUGULAR);
  m_vecItemIndex[3].push_back(PERI_ARTERY);
  m_vecItemIndex[3].push_back(PERI_VEIN);

  // 90L40J
  m_vecItemIndex[4].push_back(GLANDS);
  m_vecItemIndex[4].push_back(THYROID);
  m_vecItemIndex[4].push_back(EYE);
  m_vecItemIndex[4].push_back(SMALL_PART);
  m_vecItemIndex[4].push_back(HIP_JOINT);
  m_vecItemIndex[4].push_back(MENISCUS);
  m_vecItemIndex[4].push_back(JOINT_CAVITY);
  m_vecItemIndex[4].push_back(SPINE);
  m_vecItemIndex[4].push_back(CAROTID);
  m_vecItemIndex[4].push_back(JUGULAR);
  m_vecItemIndex[4].push_back(PERI_ARTERY);
  m_vecItemIndex[4].push_back(PERI_VEIN);

  m_vecItemIndex[5].push_back(GYN);
  m_vecItemIndex[5].push_back(EARLY_PREG);
  m_vecItemIndex[5].push_back(LATER_PREG);
  m_vecItemIndex[5].push_back(CAR_FETUS);
  m_vecItemIndex[5].push_back(KIDNEY);
  m_vecItemIndex[5].push_back(BLADDER);

  // probe6 35C20I
  if (NUM_PROBE > 5) {
    m_vecItemIndex[6].push_back(ABDO_KID);
    m_vecItemIndex[6].push_back(CAR_ADULT);
    m_vecItemIndex[6].push_back(CAR_KID);
  }

  // probe7 65C15E
  if (NUM_PROBE > 6) {
    m_vecItemIndex[7].push_back(ABDO_KID);
    m_vecItemIndex[7].push_back(CAR_ADULT);
    m_vecItemIndex[7].push_back(CAR_KID);
  }

  // probe8 30P16B
  if(NUM_PROBE > 7) {
    m_vecItemIndex[8].push_back(CAR_ADULT);
    m_vecItemIndex[8].push_back(CAR_KID);
    m_vecItemIndex[8].push_back(CAR_FETUS);
  }

  // probe9 10L25K
  if(NUM_PROBE > 8) {
    m_vecItemIndex[9].push_back(GLANDS);
    m_vecItemIndex[9].push_back(THYROID);
    m_vecItemIndex[9].push_back(EYE);
    m_vecItemIndex[9].push_back(SMALL_PART);
    m_vecItemIndex[9].push_back(CAROTID);
    m_vecItemIndex[9].push_back(JUGULAR);
    m_vecItemIndex[9].push_back(PERI_ARTERY);
    m_vecItemIndex[9].push_back(PERI_VEIN);
    m_vecItemIndex[9].push_back(HIP_JOINT);
    m_vecItemIndex[9].push_back(MENISCUS);
    m_vecItemIndex[9].push_back(JOINT_CAVITY);
    m_vecItemIndex[9].push_back(SPINE);
  }

  // probe10 60C10I
  if(NUM_PROBE > 9) {
    m_vecItemIndex[10].push_back(GYN);
  }
}

void ExamItem::WriteConfigCommon(ParaItem* paraItem, const string section, IniFile* ptrIni) {
  int texType = 0;
  int bodymarkType = 0;

  ptrIni->WriteInt(section, "Common-MBP", paraItem->common.MBP);
  ptrIni->WriteInt(section, "Common-PowerIndex", paraItem->common.powerIndex);

  ptrIni->WriteInt(section, "Common-TextType", texType);
  ptrIni->WriteInt(section, "Common-BodymarkType", bodymarkType);
  ptrIni->SyncConfigFile();
}

void ExamItem::WriteConfigOther(ParaItem* paraItem, const string section, IniFile* ptrIni) {
  // 2d
  ptrIni->WriteInt(section, "D2-DepthScale", paraItem->d2.imgScale);
  ptrIni->WriteInt(section, "D2-Gain2D", paraItem->d2.gain2D);
  ptrIni->WriteInt(section, "D2-FreqIndex", paraItem->d2.freqIndex);
  ptrIni->WriteInt(section, "D2-ScanAngle", paraItem->d2.scanAngle);
  ptrIni->WriteInt(section, "D2-SpaceCompoundIndex", paraItem->d2.spaceCompoundIndex);
  ptrIni->WriteInt(section, "D2-FreqCompoundIndex", paraItem->d2.freqCompoundIndex);
  ptrIni->WriteInt(section, "D2-Chroma", paraItem->d2.chroma);
  ptrIni->WriteInt(section, "D2-DynamicRange", paraItem->d2.dynamicRange);
  ptrIni->WriteInt(section, "D2-Harmonic", paraItem->d2.harmonic);
  ptrIni->WriteInt(section, "D2-Steer", paraItem->d2.steerIndex);
  ptrIni->WriteInt(section, "D2-TSI", paraItem->d2.TSI);
  ptrIni->WriteInt(section, "D2-LineDensity", paraItem->d2.lineDensity);
  ptrIni->WriteInt(section, "D2-EdgeEnhance", paraItem->d2.edgeEnhance);
  ptrIni->WriteInt(section, "D2-FocSum", paraItem->d2.focSum);
  ptrIni->WriteInt(section, "D2-FocPosIndex", paraItem->d2.focPosIndex);
  ptrIni->WriteInt(section, "D2-AGC", paraItem->d2.AGC);
  ptrIni->WriteInt(section, "D2-GainM", paraItem->d2.gainM);
  ptrIni->WriteInt(section, "D2-ThiFreqIndex", paraItem->d2.thiFreqIndex);
  ptrIni->WriteInt(section, "D2-LeftRight", paraItem->d2.leftRight);
  ptrIni->WriteInt(section, "D2-UpDown", paraItem->d2.upDown);
  ptrIni->WriteInt(section, "D2-Polarity", paraItem->d2.polarity);
  ptrIni->WriteInt(section, "D2-NoiseReject", paraItem->d2.noiseReject);
  ptrIni->WriteInt(section, "D2-FrameAver", paraItem->d2.frameAver);
  ptrIni->WriteInt(section, "D2-LineAver", paraItem->d2.lineAver);
  ptrIni->WriteInt(section, "D2-Smooth", paraItem->d2.smooth);
  ptrIni->WriteInt(section, "D2-Gamma", paraItem->d2.gamma);
  ptrIni->WriteInt(section, "D2-ImageEhn", paraItem->d2.imgEhn);
  ptrIni->WriteInt(section, "D2-GrayTransIndex", paraItem->d2.grayTransIndex);
  ptrIni->WriteInt(section, "D2-Scanline", paraItem->d2.scanline);

  // spectrum
  ptrIni->WriteInt(section, "Spectrum-Freq", paraItem->spectrum.freq);
  ptrIni->WriteInt(section, "Spectrum-Gain", paraItem->spectrum.gain);
  ptrIni->WriteInt(section, "Spectrum-DynamicRange", paraItem->spectrum.dynamicRange);
  ptrIni->WriteInt(section, "Spectrum-PRF", paraItem->spectrum.PRF);
  ptrIni->WriteInt(section, "Spectrum-WallFilter", paraItem->spectrum.wallFilter);
  ptrIni->WriteInt(section, "Spectrum-Invert", paraItem->spectrum.invert);
  ptrIni->WriteInt(section, "Spectrum-TimeSmooth", paraItem->spectrum.timeSmooth);
  ptrIni->WriteInt(section, "Spectrum-CorrectAngle", paraItem->spectrum.correctAngle);
  ptrIni->WriteInt(section, "Spectrum-SampleVolume", paraItem->spectrum.SV);
  ptrIni->WriteInt(section, "Spectrum-Baseline", paraItem->spectrum.baseline);
  ptrIni->WriteInt(section, "Spectrum-SpectrumSpeed", paraItem->spectrum.speed);
  ptrIni->WriteInt(section, "Spectrum-SoundVolume", paraItem->spectrum.soundVolume);
  ptrIni->WriteInt(section, "Spectrum-NoiseThread", paraItem->spectrum.noiseThread);

  // color
  ptrIni->WriteInt(section, "Color-Gain", paraItem->color.gain);
  ptrIni->WriteInt(section, "Color-PRF", paraItem->color.PRFIndex);
  ptrIni->WriteInt(section, "Color-WallFilter", paraItem->color.wallFilter);
  ptrIni->WriteInt(section, "Color-LineDensity", paraItem->color.lineDensity);
  ptrIni->WriteInt(section, "Color-Sensitive", paraItem->color.sensitive);
  ptrIni->WriteInt(section, "Color-Onflow", paraItem->color.turb);
  ptrIni->WriteInt(section, "Color-Invert", paraItem->color.invert);
  ptrIni->WriteInt(section, "Color-Reject", paraItem->color.reject);
  ptrIni->WriteInt(section, "Color-Smooth", paraItem->color.smooth);
  ptrIni->WriteInt(section, "Color-Persist", paraItem->color.persist);
  ptrIni->WriteInt(section, "Color-FlowOpt", paraItem->color.flowOpt);
  ptrIni->WriteInt(section, "Color-Baseline", paraItem->color.baseline);
  ptrIni->WriteInt(section, "Color-Steer", paraItem->color.steer);
  ptrIni->WriteInt(section, "Color-Artifact", paraItem->color.artifact);

  ptrIni->SyncConfigFile();
}

void ExamItem::ReadConfigCommon(ParaItem* paraItem, const string section, IniFile* ptrIni) {
  paraItem->common.MBP = ptrIni->ReadInt(section, "Common-MBP");
  paraItem->common.powerIndex = ptrIni->ReadInt(section, "Common-PowerIndex");
  paraItem->common.textType = ptrIni->ReadInt(section, "Common-TextType");
  paraItem->common.bodymarkType = ptrIni->ReadInt(section, "Common-BodymarkType");
}

void ExamItem::ReadConfigOther(ParaItem* paraItem, const string section, IniFile* ptrIni) {
  // 2d
  paraItem->d2.freqIndex = ptrIni->ReadInt(section, "D2-FreqIndex");
  paraItem->d2.imgScale = ptrIni->ReadInt(section, "D2-DepthScale"); //自动优化时未使用此参数
  paraItem->d2.gain2D = ptrIni->ReadInt(section, "D2-Gain2D");
  paraItem->d2.focSum = ptrIni->ReadInt(section, "D2-FocSum");
  paraItem->d2.focPosIndex = ptrIni->ReadInt(section, "D2-FocPosIndex");
  paraItem->d2.scanAngle = ptrIni->ReadInt(section, "D2-ScanAngle");
  paraItem->d2.dynamicRange = ptrIni->ReadInt(section, "D2-DynamicRange");
  paraItem->d2.lineDensity = ptrIni->ReadInt(section, "D2-LineDensity");
  paraItem->d2.steerIndex = ptrIni->ReadInt(section, "D2-Steer");
  paraItem->d2.AGC = ptrIni->ReadInt(section, "D2-AGC");
  paraItem->d2.edgeEnhance = ptrIni->ReadInt(section, "D2-EdgeEnhance");
  paraItem->d2.chroma = ptrIni->ReadInt(section, "D2-Chroma");
  paraItem->d2.leftRight = ptrIni->ReadInt(section, "D2-LeftRight");
  paraItem->d2.upDown = ptrIni->ReadInt(section, "D2-UpDown");
  paraItem->d2.polarity = ptrIni->ReadInt(section, "D2-Polarity");
  paraItem->d2.frameAver = ptrIni->ReadInt(section, "D2-FrameAver");
  paraItem->d2.lineAver = ptrIni->ReadInt(section, "D2-LineAver");
  paraItem->d2.smooth = ptrIni->ReadInt(section, "D2-Smooth");
  paraItem->d2.gamma = ptrIni->ReadInt(section, "D2-Gamma");
  paraItem->d2.noiseReject = ptrIni->ReadInt(section, "D2-NoiseReject");
  paraItem->d2.harmonic = ptrIni->ReadInt(section, "D2-Harmonic");
  paraItem->d2.TSI = ptrIni->ReadInt(section, "D2-TSI");
  paraItem->d2.imgEhn = ptrIni->ReadInt(section, "D2-ImageEhn");
  paraItem->d2.gainM = ptrIni->ReadInt(section, "D2-GainM");
  paraItem->d2.grayTransIndex = ptrIni->ReadInt(section, "D2-GrayTransIndex");
  paraItem->d2.spaceCompoundIndex = ptrIni->ReadInt(section, "D2-SpaceCompoundIndex");
  paraItem->d2.freqCompoundIndex = ptrIni->ReadInt(section, "D2-FreqCompoundIndex");
  paraItem->d2.thiFreqIndex = ptrIni->ReadInt(section, "D2-ThiFreqIndex");
  paraItem->d2.scanline = ptrIni->ReadInt(section, "D2-Scanline");

  // spectrum
  paraItem->spectrum.freq = ptrIni->ReadInt(section, "Spectrum-Freq");
  paraItem->spectrum.gain = ptrIni->ReadInt(section, "Spectrum-Gain");
  paraItem->spectrum.dynamicRange = ptrIni->ReadInt(section, "Spectrum-DynamicRange");
  paraItem->spectrum.PRF= ptrIni->ReadInt(section, "Spectrum-PRF");
  paraItem->spectrum.wallFilter = ptrIni->ReadInt(section, "Spectrum-WallFilter");
  paraItem->spectrum.invert = ptrIni->ReadInt(section, "Spectrum-Invert");
  paraItem->spectrum.timeSmooth = ptrIni->ReadInt(section, "Spectrum-TimeSmooth");
  paraItem->spectrum.correctAngle = ptrIni->ReadInt(section, "Spectrum-CorrectAngle");
  paraItem->spectrum.SV = ptrIni->ReadInt(section, "Spectrum-SampleVolume");
  paraItem->spectrum.baseline = ptrIni->ReadInt(section, "Spectrum-Baseline");
  paraItem->spectrum.speed = ptrIni->ReadInt(section, "Spectrum-SpectrumSpeed");
  paraItem->spectrum.noiseThread = ptrIni->ReadInt(section, "Spectrum-NoiseThread");
  paraItem->spectrum.soundVolume = ptrIni->ReadInt(section, "Spectrum-SoundVolume");

  // color
  paraItem->color.gain = ptrIni->ReadInt(section, "Color-Gain");
  paraItem->color.PRFIndex = ptrIni->ReadInt(section, "Color-PRF");
  paraItem->color.wallFilter = ptrIni->ReadInt(section, "Color-WallFilter");
  paraItem->color.lineDensity = ptrIni->ReadInt(section, "Color-LineDensity");
  paraItem->color.sensitive = ptrIni->ReadInt(section, "Color-Sensitive");
  paraItem->color.turb = ptrIni->ReadInt(section, "Color-Onflow");
  paraItem->color.invert = ptrIni->ReadInt(section, "Color-Invert");
  paraItem->color.reject = ptrIni->ReadInt(section, "Color-Reject");
  paraItem->color.smooth = ptrIni->ReadInt(section, "Color-Smooth");
  paraItem->color.persist = ptrIni->ReadInt(section, "Color-Persist");
  paraItem->color.artifact = ptrIni->ReadInt(section, "Color-Artifact");
  paraItem->color.flowOpt = ptrIni->ReadInt(section, "Color-FlowOpt");
  paraItem->color.baseline = ptrIni->ReadInt(section, "Color-Baseline");
  paraItem->color.steer = ptrIni->ReadInt(section, "Color-Steer");
  paraItem->color.colormap = ptrIni->ReadInt(section, "Color-ColorMap");
}

int ExamItem::GetProbeIndex(const string probeModel) {
  for (int i = 0; i < NUM_PROBE; i++) {
    if (probeModel == PROBE_LIST[i]) {
      return i;
    }
  }

  return 0;
}

int ExamItem::ReadSelectedProbeItem(const string probeModel, IniFile* ptrIni) {
  return ptrIni->ReadInt(probeModel, "SelectProbeItem");
}

void ExamItem::WriteNewExamItem(const string department, const string section, IniFile* ptrIni, string probelist, const string new_string, const string str_index) {
  string username = ViewSystem::GetInstance()->GetUserName();

  ExamItem exam;
  string userselect = exam.TransUserSelectForEng(username);

  ptrIni->WriteString(section, "UserSelect", userselect);
  ptrIni->WriteString(section, "ProbeType", probelist);
  ptrIni->WriteString(section, "ExamItem",  new_string);
  ptrIni->WriteString(section, "Department", department);
  ptrIni->WriteString(section, "GenFirstItem", str_index);
  ptrIni->SyncConfigFile();
}

void ExamItem::DeleteNewExamItem(const string department, const string section, IniFile* ptrIni) {
  ptrIni->RemoveGroup(section);
  ptrIni->SyncConfigFile();
}
