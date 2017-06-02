#ifndef __EXAM_ITEM_H__
#define __EXAM_ITEM_H__

#include "utils/IniFile.h"

#include "Def.h"
#include "probe/ProbeSocket.h"

#define NOTE_FILE                 "res/config-355/NoteSetting.ini"
#define DEFAULT_NOTE_FILE         "res/DefaultNote.ini"

#define EXAM_FILE                 "res/config-355/ItemPara.ini"
#define DEFAULT_EXAM_FILE         "res/config-355/DefaultItemPara.ini"
#define EXAM_FILE_OTHER           "res/config-355/default/ItemPara.ini"
#define EXAM_ITEM_FILE            "res/config-355/defineditem/"
#define EXAM_FILE_DIR             "res/config-355/"

#define CALC_ITEM_FILE            "res/config-355/calcitemsetting/"
#define CALC_ITEM_PATH            "res/config-355/calcitemsetting/DSCalcItemSetting.ini"
#define DEFAULT_CALC_ITEM_FILE    "res/config-355/DefaultCalcItemSetting.ini"

#define MEASURE_ITEM_FILE         "res/config-355/measureitemsetting/"
#define MEASURE_ITEM_PATH         "res/config-355/measureitemsetting/DSMeasureItemSetting.ini"
#define DEFAULT_MEASURE_ITEM_FILE "res/config-355/DefaultMeasureItemSetting.ini"

#define PROJECT_DEFAULT_FILE      "res/config-355/ProjectDefaultPara.ini"
#define PROJECT_DEBUG_FILE        "res/config-355/ProjectDebugPara.ini"

#define OPTIMIZE_FILE             "res/config-355/OptimizePara.ini"
#define DEFAULT_OPTIMIZE_FILE     "res/config-355/DefaultOptimizePara.ini"

class ExamItem {
public:
  ExamItem();
  ~ExamItem();

public:
  static const int BAND_PASS_SIZE_D2      = 5;
  static const int LOW_PASS_SIZE_D2       = 5;
  static const int WALL_FILTER_SIZE_COLOR = 3;
  static const int NUM_ITEM               = 28;

  static const string ITEM_LIB[NUM_ITEM];

public:
  // order must be indetical to ITEM_LIB[]
  enum EItem {
    ABDO_ADULT, ABDO_LIVER,   ABDO_KID,     CAR_ADULT,  CAR_KID,
    GLANDS,     THYROID,      EYE,          SMALL_PART, GYN,
    EARLY_PREG, LATER_PREG,   CAR_FETUS,    KIDNEY,     BLADDER,
    CAROTID,    JUGULAR,      PERI_ARTERY,  PERI_VEIN,  HIP_JOINT,
    MENISCUS,   JOINT_CAVITY, SPINE,        TCD,        USER1,
    USER2,      USER3,        USER4,        USERNAME,   USER
  };

  struct ParaCommon {
    int MBP;                // level of multi process
    int powerIndex;         // index of sound power
    int textType;           // typ of text(comment)
    int bodymarkType;       // type of bodymark
  };

  struct Para2D {
    int freqIndex;          // index of current pair(emit-receive) of frequency
    int imgScale;           // index of current img depth scale
    int gain2D;             // gain of 2D mode
    int focSum;             // focus sum
    int focPosIndex;        // index of focus position
    int scanAngle;          // index of scan range
    int dynamicRange;       // index of dynamic range
    int lineDensity;        // scan line density
    int steerIndex;         // index of steer, only used in Linear Probe
    int AGC;                // level of auto gain correction
    int edgeEnhance;        // level of edge enhancement
    bool harmonic;          // harmonic on/off
    int TSI;                // tissue stamp imaging
    int chroma;             // type of pseudo color
    bool leftRight;         // left or right turn
    bool upDown;            // up or down turn
    bool polarity;          // polarity turn
    int rotate;             // index of rotate angle
    int frameAver;          // level of frame average
    int lineAver;           // level of line average
    int smooth;             // level of image smooth
    int gamma;              // level of gamma correction
    int noiseReject;        // reject of noise
    int imgEhn;             // image enhancement algrithm ---
    int gainM;              // gain of M mode
    int grayTransIndex;     // gray transform
    int spaceCompoundIndex; // space compound imaging
    int freqCompoundIndex;  // freq compound imaging
    int thiFreqIndex;       // freq compound imaging
    int scanline;           // scan lines of phase probe 0:64; 1:128; 2:196; 3:256
  };

  struct ParaSpectrum {
    int freq;               // index of doppler emit frequency
    int gain;               // gain of PW mode
    int gaincw;
    int gainpw;
    int dynamicRange;       // dynamic range of PW mode
    int PRF;                // index of pulse repeat freq, control the range of velocity, not used now, determined by svn pos
    int wallFilter;         // index of wall filter in spetrum, not used now, determined by PRF
    int SV;                 // sample volume
    bool invert;            // invert of spectrum
    int timeSmooth;         // smooth in time direction
    int correctAngle;
    int speed;              // spetrum speed
    int noiseThread;
    int soundVolume;
    int baseline;
  };

  struct ParaColor {
    int gain;               // gain of CFM mode
    int PRFIndex;           // index of PRF
    int wallFilter;         // index of wall filter in color mode, not used now
    int lineDensity;        // color scan line density
    int turb;               // index of turbulence
    int sensitive;          // color flow sensitivity, not used now
    bool invert;            // invert of color, not used now
    int reject;             // reject low velocity flow
    int smooth;             // color smooth
    int persist;            // persistent of color flow
    int artifact;           // artifact of color flow
    // int gainPdi;         // gain of PDI mode, not used now
    int flowOpt;            // color flow optimize
    int baseline;           // index of baseline
    int steer;              // index of color steer
    int colormap;           // index of colormap
  };

  struct ParaItem {
    ParaCommon common;
    Para2D d2;
    ParaSpectrum spectrum;
    ParaColor color;
  };

  struct ProDefaultPara2D {
    int FilterSection;
    double BandPassW1[BAND_PASS_SIZE_D2];
    double BandPassW2[BAND_PASS_SIZE_D2];
    int BandPassWindowFunc;
    double LowPassW[LOW_PASS_SIZE_D2];
    int LowPassWindowFunc;
    int freq;
  };

  struct ProDefaultParaColor {
    int WallFilterSection;
    double WallFilterW[WALL_FILTER_SIZE_COLOR];
    int dopfreq;                        //<< doppler frequence
  };

  struct ProjectDefaultParaItem {
    int mode;
    ProDefaultPara2D pro2d;
    ProDefaultParaColor procolor;
  };

  struct ProjectDebugParaItem {
    int depth;                          // current depth
    int mode;                           // current mode
    int freq;                           // current frequence
    ProjectDefaultParaItem defaultPara; // project default paramenter
  };

public:
  vector<string> GetCurrentUserGroup();
  vector<string> GetDefaultUserGroup();
  vector<string> GetUserGroup();

  void GetUserItem(const string group, string& userselect,
    string& probelist_value, string& useritem_value, string& department_value, string& genFirstItem);
  void GetDefaultUserItem(const string group, string& userselect,
    string& probelist_value, string& useritem_value, string& department_value, string& genFirstItem);
  void DeleteUserItem(const string group);

  string TransItemName(const string str);
  string TransItemNameEng(const string str);
  string TransUserSelectForEng(const string name);

  vector<EItem> GetItemListOfProbe(const string probeModel);
  vector<string> GetUserItemListOfProbe(const string probeModel);
  string GetUserItemInfo(const string probeModel);
  string GetInitUserItemInfo(const string probeModel, const string inituseritem);
  string GetFirstGenItem();

  ExamItem::EItem GetDefaultItem(const string probeModel);
  ExamItem::ParaItem GetCurrentItemPara();
  ExamItem::ParaItem GetImgOptimize(const string probeModel);

  void GenerateDefaultExamItem();
  void GenerateDefaultImgOptimize();

  void SetUserItemOfProbe(const string probeModel, enum EItem itemIndex, const string item);
  void SetItemOfProbe(const string probeModel, enum EItem itemIndex);

  void RemoveUserFromFile(const string model, const string user, IniFile* ptrIni);
  void ReadDefaultExamItem(int probeIndex, int itemIndex, ParaItem* paraItem, const string itemName);
  void DeleteItemParaFile(int probeIndex, const string old_string, const string str);

  int ReadDefaultProbeDefaultItem(IniFile* ptrIni);
  void WriteDefaultProbeItem(IniFile* ptrIni, int item);
  void WriteSelectedProbeItem(const string probeModel, IniFile* ptrIni, int item);

  void ReadExamItemPara(int probeIndex, int itemIndex, ParaItem* paraItem, const string itemName);
  void WriteExamItemPara(int probeIndex, int itemIndex, ParaItem* paraItem, const string itemName);

  void WriteDefinedItemPara(int probeIndex, const string new_string, const string str, const string str_index);
  void WriteDefaultDefinedItemPara(int probeIndex, const string new_string, const string str, const string str_index);
  void WriteDefinedExamItemPara(const string department, string section, IniFile* ptrIni, string probelist, const string new_string, const string str_index);
  void WriteDefaultDefinedExamItemPara(const string department, string section, IniFile* ptrIni, string probelist, const string new_string, const string str_index);

  void WriteNewItemFile(int probeIndex, const string new_string, const string str, const string str_index);
  void DeleteNewItemFile(int probeIndex, const string old_string, const string str);

  void WriteNewItemToCommentFile(int probeIndex, const string new_string, const string str);
  void WriteNewItemToCalcFile(int probeIndex, const string new_string, const string str);
  void WriteNewItemToMeasureFile(int probeIndex, const string new_string, const string str);
  void DeleteNewItemForCommentFile(int probeIndex, const string old_string, const string str);
  void DeleteNewItemForCalcFile(int probeIndex, const string old_string, const string str);
  void DeleteNewItemForMeasureFile(int probeIndex, const string old_string, const string str);

  void WriteUserItemFlag(IniFile* ptrIni, bool flag);
  bool ReadUserItemFlag(IniFile* ptrIni);

  void WriteDefaultUserSelect(IniFile* ptrIni, const string username);
  string ReadDefaultUserSelect(IniFile* ptrIni);

  void WriteDefaultProbe(const string probeModel, IniFile* ptrIni);
  string ReadDefaultProbe(IniFile* ptrIni);

  void WriteDefaultUserIndex(IniFile* ptrIni, int index);
  int ReadDefaultUserIndex(IniFile* ptrIni);

  void WriteDefaultProbeItemName(IniFile* ptrIni, const string itemName);
  string ReadDefaultProbeDefaultItemName(IniFile* ptrIni);

  void WriteProjectPara(ProjectDefaultParaItem* paraItem, const string model, const string user, IniFile* ptrIni);
  vector<ExamItem::ProjectDefaultParaItem> ReadProjectPara(const string model, const string user, IniFile* ptrIni);

  void WriteProDebugParaItem(ProjectDebugParaItem* debugPara, const string model, const string user, IniFile* ptrIni);
  void ReadProDebugParaItem(ProjectDebugParaItem* debugPara, const string model, const string user, IniFile* ptrIni);

private:
  void InitItemPara(ParaItem* paraItem);
  void InitItemOfProbe();

  void WriteConfigCommon(ParaItem* paraItem, const string section, IniFile* ptrIni);
  void WriteConfigOther(ParaItem* paraItem, const string section, IniFile* ptrIni);
  void ReadConfigCommon(ParaItem* paraItem, const string section, IniFile* ptrIni);
  void ReadConfigOther(ParaItem* paraItem, const string section, IniFile* ptrIni);
  void ReadCurrentConfig(ParaItem* paraItem, const string section, IniFile* ptrIni);

  int GetProbeIndex(const string probeModel);
  int ReadSelectedProbeItem(const string probeModel, IniFile* ptrIni);

  void WriteNewExamItem(const string str, const string section, IniFile* ptrIni,string probelist, const string new_string, const string str_index);
  void DeleteNewExamItem(const string str, const string section, IniFile* ptrIni);

private:
  struct ParaDefinedItem {
    ParaCommon common;
    Para2D d2;
    ParaSpectrum spectrum;
    ParaColor color;
  };

  // store general first item
  string m_genFirstItem;

  int m_probeIndex;         // current probe
  EItem m_itemIndex;        // current item
  ParaItem m_paraItem;      // current para of item-probe
  ParaItem m_paraOptimize;  // current image optimize para

  vector<EItem> m_vecItemIndex[NUM_PROBE];  // save item list index of all support probe, example: m_itemIndex[0] save the item list of g_probe_list[0] = 35C50K
  vector<string> m_vecUserItemName; // store user defined item
};

#endif
