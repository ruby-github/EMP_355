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

  void GetUserItem(const string group, string& userselect, string& probelist_value, string& useritem_value, string& department_value, string& genFirstItem);
  void GetDefaultUserItem(const string group, string& userselect, string& probelist_value, string& useritem_value, string& department_value, string& genFirstItem);
  void DeleteUserItem(const string group);

  string TransItemName(const string str);
  string TransItemNameEng(const string str);
  string TransUserSelectForEng(const string name);

  void GetItemListOfProbe(char* probeModel, vector<enum EItem> *ptrItemList);
  void GetUserItemListOfProbe(char* probeModel, vector<string> &ItemList);

  void GetUserItemInfo(char* probeModel, string &genfirstitem);
  void GetInitUserItemInfo(char* probeModel, string inituseritem, string &genfirstitem);

  ExamItem::EItem GetDefaultItem(char* probeModel);
  void GetCurrentItemPara(ParaItem &ptrPara);
  void GetImgOptimize(char* probeModel, ParaItem &para);

  void SetUserItemOfProbe(char* probeModel, enum EItem itemIndex, const char* item);
  void SetItemOfProbe(char* probeModel, enum EItem itemIndex);

public:


public:
    // config para
    void ReadExamItemPara(int probeIndex, int itemIndex, ParaItem* paraItem,char *itemName);
    void WriteExamItemPara(int probeIndex, int itemIndex, ParaItem* paraItem,char *itemName);
    void ReadDefaultExamItem(int probeIndex, int itemIndex, ParaItem* paraItem,char *itemName);
    void WriteNewItemFile(int probeIndex, char *new_string, const char *str,const char *str_index);
    void DeleteNewItemFile(int probeIndex, const char *old_string,const char *str);
    void DeleteItemParaFile(int probeIndex, const char *old_string,const char *str);
    void WriteDefaultDefinedExamItemPara(const char *department, string section, IniFile* ptrIni, string probelist, char *new_string,const char *str_index);
    void WriteDefinedExamItemPara(const char *department, string section, IniFile* ptrIni, string probelist, char *new_string,const char *str_index);
    void WriteDefinedItemPara(int probeIndex, char *new_string, const char *str, const char *str_index);
    void WriteDefaultDefinedItemPara(int probeIndex, char *new_string,const char *str, const char *str_index);
    void WriteNewItemToCommentFile(int probeIndex, char *new_string, const char *str);
    void WriteNewItemToCalcFile(int probeIndex, char *new_string, const char *str);
    void WriteNewItemToMeasureFile(int probeIndex, char *new_string, const char *str);

    void DeleteNewItemForCommentFile(int probeIndex, const char *old_string, const char *str);
    void DeleteNewItemForCalcFile(int probeIndex, const char *old_string, const char *str);
    void DeleteNewItemForMeasureFile(int probeIndex, const char *old_string, const char *str);

    void WriteDefaultUserSelect(IniFile* ptrIni, const char *username);
    std::string ReadDefaultUserSelect(IniFile* ptrIni);
    void WriteDefaultProbe(const char *probeModel, IniFile* ptrIni);
    string ReadDefaultProbe(IniFile* ptrIni);
    void WriteDefaultUserIndex(IniFile* ptrIni, int index);

    int ReadDefaultUserIndex(IniFile* ptrIni);
    ///> only used for generate config file first
    void GenerateDefaultExamItem();
    void GenerateDefaultImgOptimize();

    //write selected item to file
    void WriteSelectedProbeItem(char *probeModel, IniFile* ptrIni, int item);

    ///>write the last probe selected item to file
    void WriteDefaultProbeItem(IniFile* ptrIni, int item);

    ///>read the last probe selectd item from file
    int ReadDefaultProbeDefaultItem(IniFile* ptrIni);

    ///>write the last probe selected item name to file
    void WriteDefaultProbeItemName(IniFile* ptrIni, const char* itemName);

    ///>read the last probe selectd item name from file
    std::string ReadDefaultProbeDefaultItemName(IniFile* ptrIni);

    ///>write user item flag
    void WriteUserItemFlag(IniFile* ptrIni, bool flag);

    ///>read user item flag
    bool ReadUserItemFlag(IniFile* ptrIni);

    vector<ExamItem::ProjectDefaultParaItem> ReadProjectPara(const char* model, const char* user, IniFile* ptrIni);
    void WriteProjectPara(ProjectDefaultParaItem *paraItem, const char* model, const char* user, IniFile* ptrIni);
    void RemoveUserFromFile(const char* model, const char* user, IniFile* ptrIni);
    void ReadProDebugParaItem(ProjectDebugParaItem* debugPara, const char* model, const char* user, IniFile* ptrIni);
    void WriteProDebugParaItem(ProjectDebugParaItem* debugPara, const char* model, const char* user, IniFile* ptrIni);

    ///>get first general item in this department
    std::string GetFirstGenItem() {
        return m_genFirstItem;
    }




private:

    // static const int MAX_ITEM = 16; ///< max number of item
    //static const string ITEM_LIB[NUM_ITEM];
    static const string KEY_COMMON;
    struct ParaDefinedItem {
        ParaCommon common;
        Para2D d2;
        ParaSpectrum spectrum;
        ParaColor color;
    };

    int m_probeIndex; ///< current probe
    EItem m_itemIndex; ///< current item
    ParaItem m_paraItem; ///< current para of item-probe
    ParaItem m_paraOptimize; ///< current image optimize para

    vector<enum EItem> m_vecItemIndex[NUM_PROBE]; ///< save item list index of all support probe, example: m_itemIndex[0] save the item list of PROBE_LIST[0] = 35C50K

    //store user defined item
    vector<string> m_vecUserItemName;
    vector<string> m_vecGenFirstItem;

    //store general first item
    string m_genFirstItem;

    int GetProbeIndex(char* probeModel);

    ///> wirte config file
    void InitItemPara(ParaItem* paraItem);
    void InitItemOfProbe();
    void WriteConfigCommon(ParaItem* paraItem, string section, IniFile* ptrIni);
    void WriteConfigOther(ParaItem* paraItem, string section, IniFile* ptrIni);
    int ReadSelectedProbeItem(char *probeModel, IniFile* ptrIni);
    void ReadConfigCommon(ParaItem* paraItem, string section, IniFile* ptrIni);
    void ReadConfigOther(ParaItem* paraItem, string section, IniFile* ptrIni);
    void ReadCurrentConfig(ParaItem* paraItem, string section, IniFile* ptrIni);
    void WriteNewExamItem(const char *str, string section, IniFile* ptrIni,string probelist,const char *new_string, const char *str_index);
    void DeleteNewExamItem(const char *str, string section, IniFile* ptrIni);
};

#endif
