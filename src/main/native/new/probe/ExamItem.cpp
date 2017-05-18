#include "probe/ExamItem.h"

#include <sstream>

#include "Def.h"
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

const string ExamItem::KEY_COMMON = "Common";

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

// ---------------------------------------------------------

int ExamItem::GetProbeIndex(const string probeModel) {
  for (int i = 0; i < NUM_PROBE; i++) {
    if (probeModel == PROBE_LIST[i]) {
      return i;
    }
  }

  return 0;
}



#include "imageProc/ModeStatus.h"
#include <stdio.h>
#include <string.h>




#include <locale.h>
#include <libintl.h>
#include "imageControl/ImgPw.h"
#include "patient/FileMan.h"
//#include "ScanMode.h"







///> public func





/*
 * @brief set probe "probeModel"'s default item
 *
 * @para probeModel probe model
 * @retval value of default item
 */
enum ExamItem::EItem ExamItem::GetDefaultItem(char* probeModel) {
    //char path[256];
    //sprintf(path, "%s/%s", CFG_RES_PATH, STORE_ITEM_PATH);
    //IniFile ini(path);
    //int item = ReadSelectedProbeItem(probeModel, &ini);
    //m_itemIndex = ExamItem::EItem (item);
    //PRINTF("----m_itemIndex=%d\n", m_itemIndex);

    m_probeIndex = GetProbeIndex(probeModel);
    m_itemIndex = m_vecItemIndex[m_probeIndex][0];

    //SetItemOfProbe(probeModel,m_itemIndex);

    return m_itemIndex;
}

void ExamItem::SetUserItemOfProbe(char* probeModel, enum EItem itemIndex, const char* item) {
    m_probeIndex = GetProbeIndex(probeModel);
    m_itemIndex = itemIndex;

    // read current item para value to m_paraItem
    char path[256];

    //const gchar *name = ViewProbe::GetInstance()->GetUserName();
    if (strcmp(user_configure, "ItemPara.ini") == 0)
        sprintf(path, "%s%s", CFG_RES_PATH, EXAM_FILE);
    else
        sprintf(path, "%s%s%s", CFG_RES_PATH, EXAM_FILE_DIR, user_configure);

    IniFile ini(path);
    ReadConfigCommon(&m_paraItem, KEY_COMMON + "-" + item, &ini);
    ReadConfigOther(&m_paraItem, PROBE_LIST[m_probeIndex] + "-" + item, &ini);
}
/*
 * @brief set current probe and current item of probe, and get para value according current probe and item, save to m_paraItem
 */
void ExamItem::SetItemOfProbe(char* probeModel, enum EItem itemIndex) {
    m_probeIndex = GetProbeIndex(probeModel);
    m_itemIndex = itemIndex;

    // read current item para value to m_paraItem
    char path[256];

    if (strcmp(user_configure, "ItemPara.ini") == 0)
        sprintf(path, "%s%s", CFG_RES_PATH, EXAM_FILE);
    else
        sprintf(path, "%s%s%s", CFG_RES_PATH, EXAM_FILE_DIR, user_configure);

    IniFile ini(path);
    ReadConfigCommon(&m_paraItem, KEY_COMMON + "-" + ITEM_LIB[m_itemIndex], &ini);
    ReadConfigOther(&m_paraItem, PROBE_LIST[m_probeIndex] + "-" + ITEM_LIB[m_itemIndex], &ini);
}
/*
 * @brief get current selected item's para
 *
 * @para ptrPara item's para value saved in it
 */
void ExamItem::GetCurrentItemPara(ParaItem &ptrPara) {
    ptrPara = m_paraItem;
}

/*
 * @brief generate default config file, para value in it should be revise according to real condition, only used when the first time when "ItemPara.ini" is generate
 * note: only used for our programer
 */
void ExamItem::GenerateDefaultExamItem() {
    int i;
    int j;

    InitItemPara(&m_paraItem);
    InitItemOfProbe();

    char path[256];
    sprintf(path, "%s/%s", CFG_RES_PATH, DEFAULT_EXAM_FILE);
    IniFile ini(path);
    // write common para in each item
    for (j = 0; j < NUM_ITEM; j ++) {
        WriteConfigCommon(&m_paraItem, KEY_COMMON + "-" + ITEM_LIB[j], &ini);
    }

    // write different para in each item and each probe
    //一个探头对应一组检查项目，见m_vecItemIndex[]， 最多MAX_ITEM（16）个。
    int size;
    int itemIndex;
    for (j = 0; j < NUM_PROBE; j ++) {
        size = m_vecItemIndex[j].size(); // get the size of item in probe PROBE_LIST[j];
        for (i = 0; i < size; i ++) {
            itemIndex = m_vecItemIndex[j][i];
            WriteConfigOther(&m_paraItem, PROBE_LIST[j] + "-" + ITEM_LIB[itemIndex], &ini);
        }
    }
}

/*
 * @brief read image optimizing para of designated probe
 *
 * @para probeModel designated probe model
 */
void ExamItem::GetImgOptimize(char* probeModel, ParaItem &para) {
    char path[256];
    sprintf(path, "%s/%s", CFG_RES_PATH, OPTIMIZE_FILE);
    IniFile ini(path);

    int probeIndex = GetProbeIndex(probeModel);
    ReadConfigCommon(&m_paraOptimize, PROBE_LIST[probeIndex], &ini);
    ReadConfigOther(&m_paraOptimize, PROBE_LIST[probeIndex], &ini);

    para = m_paraOptimize;
}

/*
 * @brief read parameter of examItem in config file, according to probeIndex and itemIndex
 * @para probeIndex [in] index of probe in PROBE_LIST
 * @para itemIndex [in] index of exam item in ITEM_LIB
 * @para paraItem[out] pointer saved all parameter read from ItemPara.ini file
 */
void ExamItem::ReadExamItemPara(int probeIndex, int itemIndex, ParaItem* paraItem, char *itemName) {
    char path[256];
    const gchar *name = ViewSystem::GetInstance()->GetUserName();
    if ((strcmp(name, "System Default") != 0) && (strcmp(name, "Умолчан системы") != 0) &&
            (strcmp(name, "系统默认") != 0) && (strcmp(name, "Domyślne Systemu") != 0)  &&
            (strcmp(name, "Par défaut du sys.") != 0) && (strcmp(name, "Systemvorgabe") != 0)&& (strcmp(name, "Sistema por defecto") !=0)) {
        sprintf(path, "%s%s%s", CFG_RES_PATH, EXAM_FILE_DIR, user_configure);
        //        sprintf(path, "%s%s%s%s%s", CFG_RES_PATH, EXAM_FILE_DIR, name, ".ini");
    } else {
        sprintf(path, "%s%s", CFG_RES_PATH, EXAM_FILE);
    }

    IniFile ini(path);
    if(probeIndex>=0) {
        if(itemIndex == ExamItem::USERNAME) {
            ReadConfigCommon(paraItem, KEY_COMMON + "-" + itemName, &ini);
            ReadConfigOther(paraItem, PROBE_LIST[probeIndex] + "-" + itemName, &ini);
        } else {
            ReadConfigCommon(paraItem, KEY_COMMON + "-" + ITEM_LIB[itemIndex], &ini);
            ReadConfigOther(paraItem, PROBE_LIST[probeIndex] + "-" + ITEM_LIB[itemIndex], &ini);
        }
    }
}
/*
 * @brief write parameter of examItem to config file, according to probeIndex and itemIndex
 * @para probeIndex[in] index of probe in PROBE_LIST
 * @para itemIndex[in] index of exam item in ITEM_LIB
 * @para paraItem[out] pointer saved all parameter read from ItemPara.ini file
 */
void ExamItem::WriteExamItemPara(int probeIndex, int itemIndex, ParaItem* paraItem,char *itemName) {
    FileMan fm;
    char path[256];
    char path_other[256]; //只是用来到处默认用户时候的参数
    const gchar *name = ViewSystem::GetInstance()->GetUserName();
    if ((strcmp(name, "System Default") != 0) && (strcmp(name, "Умолчан системы") != 0) &&
            (strcmp(name, "系统默认") != 0) && (strcmp(name, "Domyślne Systemu") != 0)  &&
            (strcmp(name, "Par défaut du sys.") != 0) && (strcmp(name, "Systemvorgabe") != 0)&& (strcmp(name, "Sistema por defecto") !=0))

    {
        sprintf(path, "%s%s%s%s%s", CFG_RES_PATH, EXAM_FILE_DIR,"userconfig/", name, ".ini");
    } else {
        sprintf(path, "%s%s", CFG_RES_PATH, EXAM_FILE);
        sprintf(path_other, "%s%s", CFG_RES_PATH, EXAM_FILE_OTHER);
        FILE *fd;
        if( (fd = fopen(path_other, "r")) == NULL) {
            PRINTF("open default itempara file error\n");
            fm.CopyFile(path, path_other);
        }
        fclose(fd);
        IniFile ini_other(path_other);
        if(itemIndex==ExamItem::USERNAME) {
            WriteConfigCommon(paraItem, KEY_COMMON + "-" + itemName, &ini_other);
            WriteConfigOther(paraItem, PROBE_LIST[probeIndex] + "-" + itemName, &ini_other);
        } else {
            WriteConfigCommon(paraItem, KEY_COMMON + "-" + ITEM_LIB[itemIndex], &ini_other);
            WriteConfigOther(paraItem, PROBE_LIST[probeIndex] + "-" + ITEM_LIB[itemIndex], &ini_other);
        }
    }
    IniFile ini(path);
    if(itemIndex==ExamItem::USERNAME) {
        WriteConfigCommon(paraItem, KEY_COMMON + "-" + itemName, &ini);
        WriteConfigOther(paraItem, PROBE_LIST[probeIndex] + "-" + itemName, &ini);
    } else {
        WriteConfigCommon(paraItem, KEY_COMMON + "-" + ITEM_LIB[itemIndex], &ini);
        WriteConfigOther(paraItem, PROBE_LIST[probeIndex] + "-" + ITEM_LIB[itemIndex], &ini);
    }
}



void ExamItem::DeleteNewItemForCalcFile(int probeIndex, const char *old_string, const char *str) {
    const gchar *username = ViewSystem::GetInstance()->GetUserName();
    char path[256];
    if(strcmp(username, _("System Default")) == 0) {
        sprintf(path, "%s%s", CFG_RES_PATH, CALC_FILE);
    } else {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username, ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni= &ini;

    ptrIni->RemoveGroup(old_string);

    ptrIni->SyncConfigFile();
}

void ExamItem::DeleteNewItemForMeasureFile(int probeIndex, const char *old_string, const char *str) {
    const gchar *username = ViewSystem::GetInstance()->GetUserName();
    char path[256];
    if(strcmp(username, _("System Default")) == 0) {
        sprintf(path, "%s%s", CFG_RES_PATH, MEASURE_FILE);
    } else {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, MEASURE_PATH, username, ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni= &ini;

    ptrIni->RemoveGroup(old_string);

    ptrIni->SyncConfigFile();
}

void ExamItem::DeleteNewItemForCommentFile(int probeIndex, const char *old_string, const char *str) {
    const gchar *username = ViewSystem::GetInstance()->GetUserName();
    char path[256];
    if(strcmp(username, _("System Default")) == 0) {
        sprintf(path, "%s%s", CFG_RES_PATH, COMMENT_FILE);
    } else {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, COMMENT_PATH, username, ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni= &ini;

    ptrIni->RemoveGroup((PROBE_LIST[probeIndex] + "-" + old_string).c_str());

    ptrIni->SyncConfigFile();
}

void ExamItem::WriteNewItemToCommentFile(int probeIndex, char *new_string, const char *str) {
    const gchar *username = ViewSystem::GetInstance()->GetUserName();
    printf("username = %s\n", username);
    char path[256];
    if(strcmp(username, _("System Default")) == 0) {
        sprintf(path, "%s%s", CFG_RES_PATH, COMMENT_FILE);
    } else {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, COMMENT_PATH, username, ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni= &ini;

    ptrIni->WriteInt((PROBE_LIST[probeIndex] + "-" + new_string).c_str(), "Number", 0);

    ptrIni->WriteString((PROBE_LIST[probeIndex] + "-" + new_string).c_str(), "Department", str);

    ptrIni->SyncConfigFile();

}

void ExamItem::WriteNewItemToCalcFile(int probeIndex, char *new_string, const char *str) {
    const gchar *username = ViewSystem::GetInstance()->GetUserName();
    char path[256];
    if(strcmp(username, _("System Default")) == 0) {
        sprintf(path, "%s%s", CFG_RES_PATH, CALC_FILE);
    } else {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username, ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni= &ini;
    ptrIni->WriteInt(new_string, "Number", 0);
    ptrIni->SyncConfigFile();

}
void ExamItem::WriteNewItemToMeasureFile(int probeIndex, char *new_string, const char *str) {
    const gchar *username = ViewSystem::GetInstance()->GetUserName();
    char path[256];
    if(strcmp(username, _("System Default")) == 0) {
        sprintf(path, "%s%s", CFG_RES_PATH, MEASURE_FILE);
    } else {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, MEASURE_PATH, username, ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni= &ini;
    ptrIni->WriteInt(new_string, "Number", 0);
    ptrIni->SyncConfigFile();

}

void ExamItem::DeleteNewItemFile(int probeIndex, const char *old_string, const char *str) {
    char path[256];
    const gchar *username = ViewSystem::GetInstance()->GetUserName();
    char name[256];
    strcpy(name,username);
    if(strcmp(name,_("System Default"))==0) {
        strcpy(name, "SystemDefault");
    }
    sprintf(path, "%s%s%s%s", CFG_RES_PATH, EXAM_ITEM_FILE,name, ".ini");

    IniFile ini(path);

    DeleteNewExamItem(str, PROBE_LIST[probeIndex] + "-" + old_string, &ini);
}
void ExamItem::DeleteItemParaFile(int probeIndex, const char *old_string, const char *str) {
    FileMan fm;
    char path[256];
    char path_other[256]; //只是用来到处默认用户时候的参数
    const gchar *name = ViewSystem::GetInstance()->GetUserName();
    if ((strcmp(name, "System Default") != 0) && (strcmp(name, "Умолчан системы") != 0) &&
            (strcmp(name, "系统默认") != 0) && (strcmp(name, "Domyślne Systemu") != 0)  &&
            (strcmp(name, "Par défaut du sys.") != 0) && (strcmp(name, "Systemvorgabe") != 0)&& (strcmp(name, "Sistema por defecto") !=0)) {
        sprintf(path, "%s%s%s", CFG_RES_PATH, EXAM_FILE_DIR, user_configure);
        //    sprintf(path, "%s%s%s%s%s", CFG_RES_PATH, EXAM_FILE_DIR, name, ".ini");
    } else {
        sprintf(path, "%s%s", CFG_RES_PATH, EXAM_FILE);

        sprintf(path_other, "%s%s", CFG_RES_PATH, EXAM_FILE_OTHER);
        FILE *fd;
        if( (fd = fopen(path_other, "r")) == NULL) {
            PRINTF("open default itempara file error\n");
            fm.CopyFile(path, path_other);
        }
        fclose(fd);
        IniFile ini_other(path_other);
        DeleteNewExamItem(str, KEY_COMMON + "-" + old_string, &ini_other);
        DeleteNewExamItem(str, PROBE_LIST[probeIndex] + "-" + old_string, &ini_other);
    }
    IniFile ini(path);

    DeleteNewExamItem(str, KEY_COMMON + "-" + old_string, &ini);
    DeleteNewExamItem(str, PROBE_LIST[probeIndex] + "-" + old_string, &ini);
}

void ExamItem::WriteDefinedItemPara(int probeIndex, char *new_string,const char *str,const char *str_index) {
    char path[256];
    const gchar *name = ViewSystem::GetInstance()->GetUserName();
    if ((strcmp(name, "System Default") != 0) && (strcmp(name, "Умолчан системы") != 0) &&
            (strcmp(name, "系统默认") != 0) && (strcmp(name, "Domyślne Systemu") != 0)  &&
            (strcmp(name, "Par défaut du sys.") != 0) && (strcmp(name, "Systemvorgabe") != 0)&& (strcmp(name, "Sistema por defecto") !=0))

    {
        sprintf(path, "%s%s%s", CFG_RES_PATH, EXAM_FILE_DIR, user_configure);
    } else {
        sprintf(path, "%s%s", CFG_RES_PATH, EXAM_FILE);
    }

    IniFile ini(path);
    WriteDefinedExamItemPara(str, PROBE_LIST[probeIndex] + "-" + new_string, &ini,PROBE_LIST[probeIndex],new_string,str_index);
}

void ExamItem::WriteDefaultDefinedItemPara(int probeIndex, char *new_string,const char *str, const char *str_index) {
    char path[256];
    sprintf(path, "%s/%s", CFG_RES_PATH, DEFAULT_EXAM_FILE);
    IniFile ini(path);

    WriteDefaultDefinedExamItemPara(str, PROBE_LIST[probeIndex] + "-" + new_string, &ini,PROBE_LIST[probeIndex],new_string,str_index);
}

void ExamItem::WriteNewItemFile(int probeIndex, char *new_string,const char *str, const char *str_index) {
    const gchar *username = ViewSystem::GetInstance()->GetUserName();
    char nameuser[256];
    strcpy(nameuser, username);
    if(strcmp(nameuser,_("System Default"))==0) {
        strcpy(nameuser, "SystemDefault");
    }

    char path[256];
    sprintf(path, "%s%s%s%s", CFG_RES_PATH, EXAM_ITEM_FILE, nameuser, ".ini");

    int fd;
    fd = open(path, O_RDWR |O_CREAT, 0666);
    close(fd);

    IniFile ini(path);

    WriteNewExamItem(str, PROBE_LIST[probeIndex] + "-" + new_string, &ini,PROBE_LIST[probeIndex],new_string, str_index);

    char path1[256];
    char path_other[256];
    const gchar *name = ViewSystem::GetInstance()->GetUserName();
    if ((strcmp(name, "System Default") != 0) && (strcmp(name, "Умолчан системы") != 0) &&
            (strcmp(name, "系统默认") != 0) && (strcmp(name, "Domyślne Systemu") != 0)  &&
            (strcmp(name, "Par défaut du sys.") != 0) && (strcmp(name, "Systemvorgabe") != 0)&& (strcmp(name, "Sistema por defecto") !=0))

    {
        sprintf(path1, "%s%s%s", CFG_RES_PATH, EXAM_FILE_DIR, user_configure);
    } else {
        sprintf(path1, "%s%s", CFG_RES_PATH, EXAM_FILE);
        sprintf(path_other, "%s%s", CFG_RES_PATH, EXAM_FILE_OTHER);
        IniFile ini1(path_other);
        WriteNewExamItem(str, PROBE_LIST[probeIndex] + "-" + new_string, &ini1, PROBE_LIST[probeIndex],new_string, str_index);
    }

    IniFile ini1(path1);

    WriteNewExamItem(str, PROBE_LIST[probeIndex] + "-" + new_string, &ini1, PROBE_LIST[probeIndex],new_string, str_index);

}

void ExamItem::ReadDefaultExamItem(int probeIndex, int itemIndex, ParaItem* paraItem, char *itemName) {
    char path[256];
    sprintf(path, "%s/%s", CFG_RES_PATH, DEFAULT_EXAM_FILE);
    IniFile ini(path);
    if(itemIndex==ExamItem::USERNAME) {
        ReadConfigCommon(paraItem, KEY_COMMON + "-" + itemName, &ini);
        ReadConfigOther(paraItem, PROBE_LIST[probeIndex] + "-" + itemName, &ini);
    } else {
        ReadConfigCommon(paraItem, KEY_COMMON + "-" + ITEM_LIB[itemIndex], &ini);
        ReadConfigOther(paraItem, PROBE_LIST[probeIndex] + "-" + ITEM_LIB[itemIndex], &ini);
    }
}

void ExamItem::GenerateDefaultImgOptimize() {
    char path[256];
    sprintf(path, "%s/%s", CFG_RES_PATH, DEFAULT_OPTIMIZE_FILE);
    IniFile ini(path);
    InitItemPara(&m_paraOptimize);

    int i;
    for (i = 0; i < NUM_PROBE; i ++) {
        WriteConfigCommon(&m_paraOptimize, PROBE_LIST[i], &ini);
        WriteConfigOther(&m_paraOptimize, PROBE_LIST[i], &ini);
    }
}


void ExamItem::InitItemPara(ParaItem* paraItem) {
    // common
    paraItem->common.MBP = 0;
    paraItem->common.powerIndex = 9;
    paraItem->common.textType = 0;
    paraItem->common.bodymarkType = 0;
    //2D
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

    //spectrum
    paraItem->spectrum.freq = 0; //< index of color freq

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

    //color
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
    //probe 0 35C505 13
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

    //75L40J
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
    ///70L60J
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

    //90L40J
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

    ///probe6 35C20I
    if (NUM_PROBE > 5) {
        m_vecItemIndex[6].push_back(ABDO_KID);
        m_vecItemIndex[6].push_back(CAR_ADULT);
        m_vecItemIndex[6].push_back(CAR_KID);
    }

    ///probe7 65C15E
    if (NUM_PROBE > 6) {
        m_vecItemIndex[7].push_back(ABDO_KID);
        m_vecItemIndex[7].push_back(CAR_ADULT);
        m_vecItemIndex[7].push_back(CAR_KID);
    }
    //probe8 30P16B
    if(NUM_PROBE > 7) {
        m_vecItemIndex[8].push_back(CAR_ADULT);
        m_vecItemIndex[8].push_back(CAR_KID);
        m_vecItemIndex[8].push_back(CAR_FETUS);
    }
    ///probe9 10L25K
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
    ///probe10 60C10I
    if(NUM_PROBE > 9) {
        m_vecItemIndex[10].push_back(GYN);
    }
}

void ExamItem::WriteConfigCommon(ParaItem* paraItem, string section, IniFile* ptrIni) {
    const char* ptrSection = section.c_str();
    // common
    ptrIni->WriteInt(ptrSection, "Common-MBP", paraItem->common.MBP);
    ptrIni->WriteInt(ptrSection, "Common-PowerIndex", paraItem->common.powerIndex);
    int texType = 0, bodymarkType = 0;
    ptrIni->WriteInt(ptrSection, "Common-TextType", texType);
    ptrIni->WriteInt(ptrSection, "Common-BodymarkType", bodymarkType);
    ptrIni->SyncConfigFile();
}


void ExamItem::WriteDefinedExamItemPara(const char *department, string section, IniFile* ptrIni, string probelist, char *new_string,const char *str_index) {
    string str_name = TransItemNameEng(str_index);

    ParaItem paradefinedItem;
    char path[256];
    ParaItem* paraItem;
    sprintf(path, "%s%s", CFG_RES_PATH, DEFAULT_EXAM_FILE);
    IniFile Ini(path);

    ReadConfigCommon(&paradefinedItem, KEY_COMMON + "-" + str_name, &Ini);
    ReadConfigOther(&paradefinedItem, probelist + "-" + str_name, &Ini);
    paradefinedItem.d2.rotate = Ini.ReadInt((probelist + "-" + str_name).c_str(), "D2-Rotate");

    char path1[256];
    const gchar *name = ViewSystem::GetInstance()->GetUserName();
    if ((strcmp(name, "System Default") != 0) && (strcmp(name, "Умолчан системы") != 0) &&
            (strcmp(name, "系统默认") != 0) && (strcmp(name, "Domyślne Systemu") != 0)  &&
            (strcmp(name, "Par défaut du sys.") != 0) && (strcmp(name, "Systemvorgabe") != 0)&& (strcmp(name, "Sistema por defecto") !=0))

    {
        sprintf(path1, "%s%s%s", CFG_RES_PATH, EXAM_FILE_DIR, user_configure);
    } else {
        sprintf(path1, "%s%s", CFG_RES_PATH, EXAM_FILE);
    }

    IniFile ini(path1);
    WriteConfigCommon(&paradefinedItem, KEY_COMMON + "-" + new_string, &ini);
    WriteConfigOther(&paradefinedItem, probelist + "-" + new_string, &ini);
    ini.WriteInt((probelist + "-" + new_string).c_str(), "Spectrum-DynamicRange", paradefinedItem.spectrum.dynamicRange);
    ini.WriteInt((probelist + "-" + new_string).c_str(), "Spectrum-SampleVolume", paradefinedItem.spectrum.SV);
    ini.WriteInt((probelist + "-" + new_string).c_str(), "D2-Rotate", paradefinedItem.d2.rotate);
    ini.SyncConfigFile();
}

void ExamItem::WriteDefaultDefinedExamItemPara(const char *department, string section, IniFile* ptrIni, string probelist,char *new_string,const char *str_index) {
    string str_name = TransItemNameEng(str_index);

    char path[256];
    ParaItem paraItem;
    sprintf(path, "%s%s", CFG_RES_PATH, EXAM_FILE);
    IniFile Ini(path);

    ReadConfigCommon(&paraItem, KEY_COMMON + "-" + str_name, &Ini);
    ReadConfigOther(&paraItem, probelist + "-" + str_name, &Ini);
    paraItem.d2.rotate = Ini.ReadInt((probelist + "-" + str_name).c_str(), "D2-Rotate");

    WriteConfigCommon(&paraItem, KEY_COMMON + "-" + new_string, ptrIni);
    WriteConfigOther(&paraItem, probelist + "-" + new_string, ptrIni);
    Ini.WriteInt((probelist + "-" + new_string).c_str(), "Spectrum-DynamicRange", paraItem.spectrum.dynamicRange);
    Ini.WriteInt((probelist + "-" + new_string).c_str(), "Spectrum-SampleVolume", paraItem.spectrum.SV);
    Ini.WriteInt((probelist + "-" + new_string).c_str(), "D2-Rotate", paraItem.d2.rotate);
    Ini.SyncConfigFile();
}

void ExamItem::WriteNewExamItem(const char *department, string section, IniFile* ptrIni, string probelist, const char *new_string,const char *str_index) {
    string username = ViewSystem::GetInstance()->GetUserName();

    ExamItem exam;
    string userselect = exam.TransUserSelectForEng(username);

    ptrIni->WriteString(section, "UserSelect",userselect);
    ptrIni->WriteString(section, "ProbeType",probelist);
    ptrIni->WriteString(section, "ExamItem", new_string);
    ptrIni->WriteString(section, "Department", department);
    ptrIni->WriteString(section, "GenFirstItem", str_index);
    ptrIni->SyncConfigFile();
}

void ExamItem::DeleteNewExamItem(const char *department, string section, IniFile* ptrIni) {
    const char* ptrSection = section.c_str();
    printf("%s\n",ptrSection);
    ptrIni->RemoveGroup(ptrSection);

    ptrIni->SyncConfigFile();
}

void ExamItem::WriteConfigOther(ParaItem* paraItem, string section, IniFile* ptrIni) {
    const char* ptrSection = section.c_str();
    //2d
    ptrIni->WriteInt(ptrSection, "D2-DepthScale", paraItem->d2.imgScale);
    ptrIni->WriteInt(ptrSection, "D2-Gain2D", paraItem->d2.gain2D);
    ptrIni->WriteInt(ptrSection, "D2-FreqIndex", paraItem->d2.freqIndex);
    ptrIni->WriteInt(ptrSection, "D2-ScanAngle", paraItem->d2.scanAngle);
    ptrIni->WriteInt(ptrSection, "D2-SpaceCompoundIndex", paraItem->d2.spaceCompoundIndex);
    ptrIni->WriteInt(ptrSection, "D2-FreqCompoundIndex", paraItem->d2.freqCompoundIndex);
    ptrIni->WriteInt(ptrSection, "D2-Chroma", paraItem->d2.chroma);
    ptrIni->WriteInt(ptrSection, "D2-DynamicRange", paraItem->d2.dynamicRange);
    ptrIni->WriteInt(ptrSection, "D2-Harmonic", paraItem->d2.harmonic);
    ptrIni->WriteInt(ptrSection, "D2-Steer", paraItem->d2.steerIndex);
    ptrIni->WriteInt(ptrSection, "D2-TSI", paraItem->d2.TSI);
    ptrIni->WriteInt(ptrSection, "D2-LineDensity", paraItem->d2.lineDensity);
    ptrIni->WriteInt(ptrSection, "D2-EdgeEnhance", paraItem->d2.edgeEnhance);
    ptrIni->WriteInt(ptrSection, "D2-FocSum", paraItem->d2.focSum);
    ptrIni->WriteInt(ptrSection, "D2-FocPosIndex", paraItem->d2.focPosIndex);
    ptrIni->WriteInt(ptrSection, "D2-AGC", paraItem->d2.AGC);
    ptrIni->WriteInt(ptrSection, "D2-GainM", paraItem->d2.gainM);
    ptrIni->WriteInt(ptrSection, "D2-ThiFreqIndex", paraItem->d2.thiFreqIndex);
    ptrIni->WriteInt(ptrSection, "D2-LeftRight", paraItem->d2.leftRight);
    ptrIni->WriteInt(ptrSection, "D2-UpDown", paraItem->d2.upDown);
    ptrIni->WriteInt(ptrSection, "D2-Polarity", paraItem->d2.polarity);
    ptrIni->WriteInt(ptrSection, "D2-NoiseReject", paraItem->d2.noiseReject);
    //ptrIni->WriteInt(ptrSection, "D2-Rotate", paraItem->d2.rotate);
    ptrIni->WriteInt(ptrSection, "D2-FrameAver", paraItem->d2.frameAver);
    ptrIni->WriteInt(ptrSection, "D2-LineAver", paraItem->d2.lineAver);
    ptrIni->WriteInt(ptrSection, "D2-Smooth", paraItem->d2.smooth);
    ptrIni->WriteInt(ptrSection, "D2-Gamma", paraItem->d2.gamma);
    ptrIni->WriteInt(ptrSection, "D2-ImageEhn", paraItem->d2.imgEhn);
    ptrIni->WriteInt(ptrSection, "D2-GrayTransIndex", paraItem->d2.grayTransIndex);
    ptrIni->WriteInt(ptrSection, "D2-Scanline", paraItem->d2.scanline);

    // spectrum
    ptrIni->WriteInt(ptrSection, "Spectrum-Freq", paraItem->spectrum.freq);
    ptrIni->WriteInt(ptrSection, "Spectrum-Gain", paraItem->spectrum.gain);
    ptrIni->WriteInt(ptrSection, "Spectrum-DynamicRange", paraItem->spectrum.dynamicRange);
    ptrIni->WriteInt(ptrSection, "Spectrum-PRF", paraItem->spectrum.PRF);
    ptrIni->WriteInt(ptrSection, "Spectrum-WallFilter", paraItem->spectrum.wallFilter);
    ptrIni->WriteInt(ptrSection, "Spectrum-Invert", paraItem->spectrum.invert);
    ptrIni->WriteInt(ptrSection, "Spectrum-TimeSmooth", paraItem->spectrum.timeSmooth); //time resolution
    ptrIni->WriteInt(ptrSection, "Spectrum-CorrectAngle", paraItem->spectrum.correctAngle);
    ptrIni->WriteInt(ptrSection, "Spectrum-SampleVolume", paraItem->spectrum.SV);
    ptrIni->WriteInt(ptrSection, "Spectrum-Baseline", paraItem->spectrum.baseline);
    ptrIni->WriteInt(ptrSection, "Spectrum-SpectrumSpeed", paraItem->spectrum.speed);
    ptrIni->WriteInt(ptrSection, "Spectrum-SoundVolume", paraItem->spectrum.soundVolume);
    ptrIni->WriteInt(ptrSection, "Spectrum-NoiseThread", paraItem->spectrum.noiseThread);

    // color
    ptrIni->WriteInt(ptrSection, "Color-Gain", paraItem->color.gain);
    ptrIni->WriteInt(ptrSection, "Color-PRF", paraItem->color.PRFIndex);
    ptrIni->WriteInt(ptrSection, "Color-WallFilter", paraItem->color.wallFilter);
    ptrIni->WriteInt(ptrSection, "Color-LineDensity", paraItem->color.lineDensity);
    ptrIni->WriteInt(ptrSection, "Color-Sensitive", paraItem->color.sensitive);
    ptrIni->WriteInt(ptrSection, "Color-Onflow", paraItem->color.turb);
    ptrIni->WriteInt(ptrSection, "Color-Invert", paraItem->color.invert);
    ptrIni->WriteInt(ptrSection, "Color-Reject", paraItem->color.reject);
    ptrIni->WriteInt(ptrSection, "Color-Smooth", paraItem->color.smooth);
    ptrIni->WriteInt(ptrSection, "Color-Persist", paraItem->color.persist);
    ptrIni->WriteInt(ptrSection, "Color-FlowOpt", paraItem->color.flowOpt);
    ptrIni->WriteInt(ptrSection, "Color-Baseline", paraItem->color.baseline);
    ptrIni->WriteInt(ptrSection, "Color-Steer", paraItem->color.steer);
    //ptrIni->WriteInt(ptrSection, "Color-ColorMap", paraItem->color.colormap);
    ptrIni->WriteInt(ptrSection, "Color-Artifact", paraItem->color.artifact);

    ptrIni->SyncConfigFile();
}

void ExamItem::ReadConfigCommon(ParaItem* paraItem, string section, IniFile* ptrIni) {
    const char* ptrSection = section.c_str();
//    printf("ptrsection=%s\n",ptrSection);
    // common
    paraItem->common.MBP = ptrIni->ReadInt(ptrSection, "Common-MBP");
    paraItem->common.powerIndex = ptrIni->ReadInt(ptrSection, "Common-PowerIndex");
    paraItem->common.textType = ptrIni->ReadInt(ptrSection, "Common-TextType");
    paraItem->common.bodymarkType = ptrIni->ReadInt(ptrSection, "Common-BodymarkType");
}

void ExamItem::ReadConfigOther(ParaItem* paraItem, string section, IniFile* ptrIni) {
    const char* ptrSection = section.c_str();

    PRINTF("==========================item: section is %s\n", ptrSection);
    //2d
    paraItem->d2.freqIndex = ptrIni->ReadInt(ptrSection, "D2-FreqIndex");
    paraItem->d2.imgScale = ptrIni->ReadInt(ptrSection, "D2-DepthScale"); //自动优化时未使用此参数
    paraItem->d2.gain2D = ptrIni->ReadInt(ptrSection, "D2-Gain2D");
    paraItem->d2.focSum = ptrIni->ReadInt(ptrSection, "D2-FocSum");
    paraItem->d2.focPosIndex = ptrIni->ReadInt(ptrSection, "D2-FocPosIndex");
    paraItem->d2.scanAngle = ptrIni->ReadInt(ptrSection, "D2-ScanAngle");
    paraItem->d2.dynamicRange = ptrIni->ReadInt(ptrSection, "D2-DynamicRange");
    paraItem->d2.lineDensity = ptrIni->ReadInt(ptrSection, "D2-LineDensity");
    paraItem->d2.steerIndex = ptrIni->ReadInt(ptrSection, "D2-Steer");
    paraItem->d2.AGC = ptrIni->ReadInt(ptrSection, "D2-AGC");
    paraItem->d2.edgeEnhance = ptrIni->ReadInt(ptrSection, "D2-EdgeEnhance");
    paraItem->d2.chroma = ptrIni->ReadInt(ptrSection, "D2-Chroma");
    paraItem->d2.leftRight = ptrIni->ReadInt(ptrSection, "D2-LeftRight");
    paraItem->d2.upDown = ptrIni->ReadInt(ptrSection, "D2-UpDown");
    paraItem->d2.polarity = ptrIni->ReadInt(ptrSection, "D2-Polarity");
    //paraItem->d2.rotate = ptrIni->ReadInt(ptrSection, "D2-Rotate");
    paraItem->d2.frameAver = ptrIni->ReadInt(ptrSection, "D2-FrameAver");
    paraItem->d2.lineAver = ptrIni->ReadInt(ptrSection, "D2-LineAver");
    paraItem->d2.smooth = ptrIni->ReadInt(ptrSection, "D2-Smooth");
    paraItem->d2.gamma = ptrIni->ReadInt(ptrSection, "D2-Gamma");
    paraItem->d2.noiseReject = ptrIni->ReadInt(ptrSection, "D2-NoiseReject");
    paraItem->d2.harmonic = ptrIni->ReadInt(ptrSection, "D2-Harmonic");
    paraItem->d2.TSI = ptrIni->ReadInt(ptrSection, "D2-TSI");
    paraItem->d2.imgEhn = ptrIni->ReadInt(ptrSection, "D2-ImageEhn");
    paraItem->d2.gainM = ptrIni->ReadInt(ptrSection, "D2-GainM");
    paraItem->d2.grayTransIndex = ptrIni->ReadInt(ptrSection, "D2-GrayTransIndex");
    paraItem->d2.spaceCompoundIndex = ptrIni->ReadInt(ptrSection, "D2-SpaceCompoundIndex");
    paraItem->d2.freqCompoundIndex = ptrIni->ReadInt(ptrSection, "D2-FreqCompoundIndex");
    paraItem->d2.thiFreqIndex = ptrIni->ReadInt(ptrSection, "D2-ThiFreqIndex");
    paraItem->d2.scanline = ptrIni->ReadInt(ptrSection, "D2-Scanline");

    // spectrum
    paraItem->spectrum.freq = ptrIni->ReadInt(ptrSection, "Spectrum-Freq");
    paraItem->spectrum.gain = ptrIni->ReadInt(ptrSection, "Spectrum-Gain");
    paraItem->spectrum.dynamicRange = ptrIni->ReadInt(ptrSection, "Spectrum-DynamicRange");
    paraItem->spectrum.PRF= ptrIni->ReadInt(ptrSection, "Spectrum-PRF");
    paraItem->spectrum.wallFilter = ptrIni->ReadInt(ptrSection, "Spectrum-WallFilter");
    paraItem->spectrum.invert = ptrIni->ReadInt(ptrSection, "Spectrum-Invert");
    paraItem->spectrum.timeSmooth = ptrIni->ReadInt(ptrSection, "Spectrum-TimeSmooth");
    paraItem->spectrum.correctAngle = ptrIni->ReadInt(ptrSection, "Spectrum-CorrectAngle");
    paraItem->spectrum.SV = ptrIni->ReadInt(ptrSection, "Spectrum-SampleVolume");
    paraItem->spectrum.baseline = ptrIni->ReadInt(ptrSection, "Spectrum-Baseline");
    paraItem->spectrum.speed = ptrIni->ReadInt(ptrSection, "Spectrum-SpectrumSpeed");
    paraItem->spectrum.noiseThread = ptrIni->ReadInt(ptrSection, "Spectrum-NoiseThread");
    paraItem->spectrum.soundVolume = ptrIni->ReadInt(ptrSection, "Spectrum-SoundVolume");

    // color
    paraItem->color.gain = ptrIni->ReadInt(ptrSection, "Color-Gain");
    paraItem->color.PRFIndex = ptrIni->ReadInt(ptrSection, "Color-PRF");
    paraItem->color.wallFilter = ptrIni->ReadInt(ptrSection, "Color-WallFilter");
    paraItem->color.lineDensity = ptrIni->ReadInt(ptrSection, "Color-LineDensity");
    paraItem->color.sensitive = ptrIni->ReadInt(ptrSection, "Color-Sensitive");
    paraItem->color.turb = ptrIni->ReadInt(ptrSection, "Color-Onflow");
    paraItem->color.invert = ptrIni->ReadInt(ptrSection, "Color-Invert");
    paraItem->color.reject = ptrIni->ReadInt(ptrSection, "Color-Reject");
    paraItem->color.smooth = ptrIni->ReadInt(ptrSection, "Color-Smooth");
    paraItem->color.persist = ptrIni->ReadInt(ptrSection, "Color-Persist");
    paraItem->color.artifact = ptrIni->ReadInt(ptrSection, "Color-Artifact");
    paraItem->color.flowOpt = ptrIni->ReadInt(ptrSection, "Color-FlowOpt");
    paraItem->color.baseline = ptrIni->ReadInt(ptrSection, "Color-Baseline");
    paraItem->color.steer = ptrIni->ReadInt(ptrSection, "Color-Steer");
    paraItem->color.colormap = ptrIni->ReadInt(ptrSection, "Color-ColorMap");
}

void ExamItem::WriteSelectedProbeItem(char *probeModel, IniFile* ptrIni, int item) {
    // write item to file
    ptrIni->WriteInt(probeModel, "SelectProbeItem", item);
    ptrIni->SyncConfigFile();
}

int ExamItem::ReadSelectedProbeItem(char *probeModel, IniFile* ptrIni) {
    // read item from file
    return (ptrIni->ReadInt(probeModel, "SelectProbeItem"));
}
void ExamItem::WriteDefaultProbe(const char *probeModel, IniFile* ptrIni) {
    ptrIni->WriteString("ProbeModel", "ProbeModel", probeModel);
    ptrIni->SyncConfigFile();
}

void ExamItem::WriteDefaultUserIndex(IniFile* ptrIni, int index) {
    ptrIni->WriteInt("UserIndex", "DefaultUserIndex", index);
    ptrIni->SyncConfigFile();
}

int ExamItem::ReadDefaultUserIndex(IniFile* ptrIni) {
    return (ptrIni->ReadInt("UserIndex", "DefaultUserIndex"));
}

void ExamItem::WriteDefaultUserSelect(IniFile* ptrIni, const char *username) {
    ptrIni->WriteString("UserSelect", "DefaultUser", username);
    ptrIni->SyncConfigFile();
}

std::string ExamItem::ReadDefaultUserSelect(IniFile* ptrIni) {
    return (ptrIni->ReadString("UserSelect", "DefaultUser"));
}

string ExamItem::ReadDefaultProbe(IniFile* ptrIni) {
    // read default probe from file
    return (ptrIni->ReadString("ProbeModel", "ProbeModel"));
}

void ExamItem::WriteDefaultProbeItem(IniFile* ptrIni, int item) {
    ptrIni->WriteInt("Item", "DefaultItem", item);
    ptrIni->SyncConfigFile();
}

int ExamItem::ReadDefaultProbeDefaultItem(IniFile* ptrIni) {
    return (ptrIni->ReadInt("Item", "DefaultItem"));
}

void ExamItem::WriteDefaultProbeItemName(IniFile* ptrIni, const char *itemName) {
    ptrIni->WriteString("UserItemName", "ItemName", itemName);
    ptrIni->SyncConfigFile();
}

std::string ExamItem::ReadDefaultProbeDefaultItemName(IniFile* ptrIni) {
    return (ptrIni->ReadString("UserItemName", "ItemName"));
}

void ExamItem::WriteUserItemFlag(IniFile* ptrIni, bool flag) {
    ptrIni->WriteBool("UserItemFlag", "UserFlag", flag);
    ptrIni->SyncConfigFile();
}

bool ExamItem::ReadUserItemFlag(IniFile* ptrIni) {
    return (ptrIni->ReadBool("UserItemFlag", "UserFlag"));
}

vector<ExamItem::ProjectDefaultParaItem> ExamItem::ReadProjectPara(const char* model, const char* user, IniFile* ptrIni) {
    vector<ExamItem::ProjectDefaultParaItem> projectPara;
    if(!projectPara.empty())
        projectPara.clear();

    if(model == NULL || user == NULL) return projectPara;

    int index;
    ExamItem::ProjectDefaultParaItem para;
    char group_name[256]= {"\0"};
    char key_name[256]= {"\0"};
    //printf("++++++model:%s\n", model);

    int j = 0;
    int size;
    for(int i = 0; i < NUM_PROBE; i++)
        if(strcmp(model, PROBE_LIST[i].c_str()) == 0) {
            index = i;
            int freq_2d_sum = ProbeSocket::FREQ2D_SUM[index];
            int freq_doppler_sum = ProbeSocket::FREQ_DOPPLER_SUM[index];
            sprintf(group_name, "%s-%s", PROBE_LIST[index].c_str(), "Default");
            for(int i = 0; i < ProbeSocket::FREQ2D_SUM[index]; i++) {
                sprintf(key_name, "%s-%d-%s", "D2", ProbeSocket::FREQ2D[index][i].emit, "FilterSection");
                para.pro2d.FilterSection = ptrIni->ReadInt(group_name, key_name);
                sprintf(key_name, "%s-%d-%s", "D2", ProbeSocket::FREQ2D[index][i].emit, "BandPassW1");
                size = ptrIni->ReadDoubleList(group_name, key_name).size();
                if(size >= ExamItem::BAND_PASS_SIZE_D2) {
                    size = ExamItem::BAND_PASS_SIZE_D2;
                    for(j = 0; j < size; j++) {
                        para.pro2d.BandPassW1[j] = ptrIni->ReadDoubleList(group_name, key_name)[j];
                        //printf("+++%d----%.1f\n", j, para.pro2d.BandPassW1[j]);
                    }
                } else {
                    for(j = 0; j < size; j++) {
                        para.pro2d.BandPassW1[j] = ptrIni->ReadDoubleList(group_name, key_name)[j];
                        //printf("+++%d----%.1f\n", j, para.pro2d.BandPassW1[j]);
                    }
                    for(j = size; j < ExamItem::BAND_PASS_SIZE_D2; j++)
                        para.pro2d.BandPassW1[j] = 0;
                }
                sprintf(key_name, "%s-%d-%s", "D2", ProbeSocket::FREQ2D[index][i].emit, "BandPassW2");
                size = ptrIni->ReadDoubleList(group_name, key_name).size();
                if(size >= ExamItem::BAND_PASS_SIZE_D2) {
                    size = ExamItem::BAND_PASS_SIZE_D2;
                    for(j = 0; j < size; j++) {
                        para.pro2d.BandPassW2[j] = ptrIni->ReadDoubleList(group_name, key_name)[j];
                        //printf("+++%d----%.1f\n", j, para.pro2d.BandPassW2[j]);
                    }
                } else {
                    for(j = 0; j < size; j++) {
                        para.pro2d.BandPassW2[j] = ptrIni->ReadDoubleList(group_name, key_name)[j];
                        //printf("+++%d----%.1f\n", j, para.pro2d.BandPassW2[j]);
                    }
                    for(j = size; j < ExamItem::BAND_PASS_SIZE_D2; j++)
                        para.pro2d.BandPassW2[j] = 0;
                }
                sprintf(key_name, "%s-%d-%s", "D2", ProbeSocket::FREQ2D[index][i].emit, "BandPassWindowFunc");
                para.pro2d.BandPassWindowFunc = ptrIni->ReadInt(group_name, key_name);
                sprintf(key_name, "%s-%d-%s", "D2", ProbeSocket::FREQ2D[index][i].emit, "LowPassW");
                size = ptrIni->ReadDoubleList(group_name, key_name).size();
                if(size >= ExamItem::LOW_PASS_SIZE_D2) {
                    size = ExamItem::LOW_PASS_SIZE_D2;
                    for(j = 0; j < size; j++) {
                        para.pro2d.LowPassW[j] = ptrIni->ReadDoubleList(group_name, key_name)[j];
                        //printf("+++%d----%.1f\n", j, para.pro2d.LowPassW[j]);
                    }
                } else {
                    for(j = 0; j < size; j++) {
                        para.pro2d.LowPassW[j] = ptrIni->ReadDoubleList(group_name, key_name)[j];
                        //printf("+++%d----%.1f\n", j, para.pro2d.LowPassW[j]);
                    }
                    for(j = size; j < ExamItem::BAND_PASS_SIZE_D2; j++)
                        para.pro2d.LowPassW[j] = 0;
                }
                sprintf(key_name, "%s-%d-%s", "D2", ProbeSocket::FREQ2D[index][i].emit, "LowPassWindowFunc");
                para.pro2d.LowPassWindowFunc = ptrIni->ReadInt(group_name, key_name);
                //cfm
                if(i>ProbeSocket::FREQ_DOPPLER_SUM[index]-1) {
                    projectPara.push_back(para);
                    continue;
                }
                sprintf(key_name, "%s-%d-%s", "Color", ProbeSocket::FREQ_DOPPLER[index][i], "WallFilterSection");
                para.procolor.WallFilterSection = ptrIni->ReadInt(group_name, key_name);
                sprintf(key_name, "%s-%d-%s", "Color", ProbeSocket::FREQ_DOPPLER[index][i], "WallFilterW");
                size = ptrIni->ReadDoubleList(group_name, key_name).size();
                if(size >= ExamItem::WALL_FILTER_SIZE_COLOR) {
                    size = ExamItem::WALL_FILTER_SIZE_COLOR;
                    for(j = 0; j < size; j++) {
                        para.procolor.WallFilterW[j] = ptrIni->ReadDoubleList(group_name, key_name)[j];
                        //printf("+++%d----%.1f\n", j, para.procolor.WallFilterW[j]);
                    }
                } else {
                    for(j = 0; j < size; j++) {
                        para.procolor.WallFilterW[j] = ptrIni->ReadDoubleList(group_name, key_name)[j];
                        PRINTF("+++%d----%.1f\n", j, para.procolor.WallFilterW[j]);
                    }
                    for(j = size; j < ExamItem::WALL_FILTER_SIZE_COLOR; j++)
                        para.procolor.WallFilterW[j] = 0;
                }
            }
        }

    return projectPara;
}

void ExamItem::WriteProjectPara(ExamItem::ProjectDefaultParaItem *proParaItem, const char* model, const char* user, IniFile* ptrIni) {
    if(model == NULL || user == NULL) return;

    char group_name[256] = {"\0"};
    char key_name[256] = {"\0"};
    sprintf(group_name, "%s-%s", model, user);
    if((ScanMode::EScanMode)proParaItem->mode == ScanMode::D2) {
        sprintf(key_name, "%s-%d-%s", "D2", proParaItem->pro2d.freq, "FilterSection");
        ptrIni->WriteInt(group_name, key_name, proParaItem->pro2d.FilterSection);
        sprintf(key_name, "%s-%d-%s", "D2", proParaItem->pro2d.freq, "BandPassW1");
        char *tmp1 = group_name;
        char *tmp2 = key_name;
        ptrIni->WriteDoubleList(tmp1, tmp2, (double*)proParaItem->pro2d.BandPassW1, ExamItem::BAND_PASS_SIZE_D2);
        sprintf(key_name, "%s-%d-%s", "D2", proParaItem->pro2d.freq, "BandPassW2");
        tmp2 = key_name;
        ptrIni->WriteDoubleList(tmp1, tmp2, (double*)proParaItem->pro2d.BandPassW2, ExamItem::BAND_PASS_SIZE_D2);
        sprintf(key_name, "%s-%d-%s", "D2", proParaItem->pro2d.freq, "BandPassWindowFunc");
        tmp2 = key_name;
        ptrIni->WriteInt(group_name, key_name, proParaItem->pro2d.BandPassWindowFunc);
        sprintf(key_name, "%s-%d-%s", "D2", proParaItem->pro2d.freq, "LowPassW");
        tmp2 = key_name;
        ptrIni->WriteDoubleList(tmp1, tmp2, (double*)proParaItem->pro2d.LowPassW, ExamItem::LOW_PASS_SIZE_D2);
        sprintf(key_name, "%s-%d-%s", "D2", proParaItem->pro2d.freq, "LowPassWindowFunc");
        tmp2 = key_name;
        ptrIni->WriteInt(group_name, key_name, proParaItem->pro2d.LowPassWindowFunc);
    } else if((ScanMode::EScanMode)proParaItem->mode == ScanMode::CFM) {
        sprintf(key_name, "%s-%d-%s", "Color", proParaItem->procolor.dopfreq, "WallFilterSection");
        ptrIni->WriteInt(group_name, (char*)key_name, proParaItem->procolor.WallFilterSection);
        sprintf(key_name, "%s-%d-%s", "Color", proParaItem->procolor.dopfreq, "WallFilterW");
        ptrIni->WriteDoubleList(group_name, (char*)key_name, (double*)proParaItem->procolor.WallFilterW, ExamItem::WALL_FILTER_SIZE_COLOR);
    }

    ptrIni->SyncConfigFile();
}

void ExamItem::RemoveUserFromFile(const char* model, const char* user, IniFile* ptrIni) {
    if(model == NULL || user == NULL) {
        perror("input is null\n");
        return;
    }
    char group_name[256] = {"\0"};
    sprintf(group_name, "%s-%s", model, user);
    ptrIni->RemoveGroup((char*)group_name);
    ptrIni->SyncConfigFile();
}

void ExamItem::ReadProDebugParaItem(ProjectDebugParaItem* debugPara, const char* model, const char* user, IniFile* ptrIni) {
    char group_name[256] = {"\0"};
    char key_name[256]= {"\0"};
    sprintf(group_name, "%s-%s", model, user);
    vector<double> value;
    if(!value.empty())
        value.clear();
    debugPara->depth = ptrIni->ReadInt(group_name, "depth");
    debugPara->mode = ptrIni->ReadInt(group_name, "mode");
    debugPara->freq = ptrIni->ReadInt(group_name, "freq");
    if((ScanMode::EScanMode)debugPara->mode == ScanMode::D2) {
        sprintf(key_name, "%s-%d-%s", "D2", debugPara->freq, "FilterSection");
        debugPara->defaultPara.pro2d.FilterSection = ptrIni->ReadInt(group_name, key_name);
        //bandPassW1
        sprintf(key_name, "%s-%d-%s", "D2", debugPara->freq, "BandPassW1");
        value = ptrIni->ReadDoubleList((char*)group_name, (char*)key_name);
        for(int i = 0; i < BAND_PASS_SIZE_D2; i++)
            debugPara->defaultPara.pro2d.BandPassW1[i] = value[i];
        //bandPassW2
        sprintf(key_name, "%s-%d-%s", "D2", debugPara->freq, "BandPassW2");
        value = ptrIni->ReadDoubleList((char*)group_name, (char*)key_name);
        for(int i = 0; i < BAND_PASS_SIZE_D2; i++)
            debugPara->defaultPara.pro2d.BandPassW2[i] = value[i];
        //bandPassWindowFunc
        sprintf(key_name, "%s-%d-%s", "D2", debugPara->freq, "BandPassWindowFunc");
        debugPara->defaultPara.pro2d.BandPassWindowFunc = ptrIni->ReadInt(group_name, key_name);
        //lowPassW
        sprintf(key_name, "%s-%d-%s", "D2", debugPara->freq, "LowPassW");
        value = ptrIni->ReadDoubleList((char*)group_name, (char*)key_name);
        for(int i = 0; i < BAND_PASS_SIZE_D2; i++)
            debugPara->defaultPara.pro2d.LowPassW[i] = value[i];
        //LowPassWindowFunc
        sprintf(key_name, "%s-%d-%s", "D2", debugPara->freq, "LowPassWindowFunc");
        debugPara->defaultPara.pro2d.LowPassWindowFunc = ptrIni->ReadInt(group_name, key_name);
    } else if((ScanMode::EScanMode)debugPara->mode == ScanMode::CFM) {
        sprintf(key_name, "%s-%d-%s", "Color", debugPara->freq, "WallFilterSection");
        debugPara->defaultPara.procolor.WallFilterSection = ptrIni->ReadInt(group_name, (char*)key_name);
        sprintf(key_name, "%s-%d-%s", "Color", debugPara->freq, "WallFilterW");
        value = ptrIni->ReadDoubleList((char*)group_name, (char*)key_name);
        for(int i = 0; i < WALL_FILTER_SIZE_COLOR; i++)
            debugPara->defaultPara.procolor.WallFilterW[i] = value[i];
    }
}

void ExamItem::WriteProDebugParaItem(ProjectDebugParaItem* debugPara, const char* model, const char* user, IniFile* ptrIni) {
    if(model == NULL || user == NULL) return;

    char group_name[256] = {"\0"};
    sprintf(group_name, "%s-%s", model, user);
    ptrIni->WriteInt((char*)group_name, "depth", debugPara->depth);
    ptrIni->WriteInt((char*)group_name, "mode", debugPara->mode);
    ptrIni->WriteInt((char*)group_name, "freq", debugPara->freq);
    WriteProjectPara(&debugPara->defaultPara, model, user, ptrIni);
}
