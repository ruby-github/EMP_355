#include "probe/ProbeSelect.h"

#include "utils/Const.h"

#include "calcPeople/MenuCalcNew.h"
#include "imageProc/ImgProc2D.h"
#include "imageProc/ImgProcM.h"
#include "imageProc/ImgProcPw.h"
#include "imageProc/ImgProcCfm.h"
#include "imageControl/Img2D.h"
#include "imageControl/ImgPw.h"
#include "imageControl/ImgCfm.h"
#include "imageProc/GlobalClassMan.h"
#include "imageControl/Update2D.h"
#include "keyboard/KeyFunc.h"
#include "keyboard/KeyValueOpr.h"
#include "keyboard/LightDef.h"
#include "measure/MenuMeasure.h"
#include "probe/BiopsyLine.h"
#include "probe/ViewProbe.h"

extern bool g_init;

int ProbeSelect::m_socketIndex = 0;
ExamItem::EItem ProbeSelect::m_itemIndex = ExamItem::ABDO_ADULT;

// ---------------------------------------------------------

ExamItem::EItem ProbeSelect::GetItemIndex() {
  return m_itemIndex;
}

ProbeSelect::ProbeSelect() {
  m_ptrProbe = ProbeMan::GetInstance();
}

ProbeSelect::~ProbeSelect() {
}

bool ProbeSelect::Execute() {
  PRINTF("probe select\n");
  FreezeMode::GetInstance()->Freeze();

  Update2D::SetCineRemoveImg(-1);

  // get probe and item info
  if (!ProbeRead()) {
    return false;
  }

  // display probe dialog and wait user's operation
  ProbeInit(1, ExamItem::GYN);

  return true;
}

bool ProbeSelect::ProbeRead() {
  int select = MAX_SOCKET;

  // power off HV
  m_ptrProbe->ActiveHV(FALSE);

  // read probe
  m_ptrProbe->GetAllProbe(m_para);

  // get exam item list
  for (int i = 0; i < MAX_SOCKET; i ++) {
    if (m_para[i].exist) {
      PRINTF("================get Probe Model correct: %s\n", m_para[i].model);
      m_itemList[i] = m_e.GetItemListOfProbe(m_para[i].model);
      select = i;
    } else {
      m_itemList[i].clear();
    }
  }

  if (select == MAX_SOCKET) {
    return false;
  } else {
    return true;
  }
}

void ProbeSelect::UserItemOfProbeInit(int indexSocket, ExamItem::EItem indexItem, const string item) {
  // exit auto optimize if in it
  KeyAutoOptimize::AutoOff();

  // exit local zoom if in it
  KeyLocalZoom klz;
  klz.ExitLocalZoom();

  // get real probe and item parameter
  m_ptrProbe->SetProbeSocket(indexSocket);
  ProbeSocket::ProbePara curPara;
  m_ptrProbe->GetCurProbe(curPara);

  if (g_init) {
    IniFile ini(string(CFG_RES_PATH) + string(STORE_DEFAULT_ITEM_PATH));

    ExamItem exam;
    string userItemName = exam.ReadDefaultProbeDefaultItemName(&ini);

    // keeping calc and bodymark are right
    string geninitfirstitem = exam.GetInitUserItemInfo(curPara.model, userItemName);
    int probeIndex = indexSocket;

    if (probeIndex < MAX_SOCKET) {
      for (int i = 0; i < (int)m_itemList[probeIndex].size(); i++) {
        PRINTF("str_firesrt_item=%s, examlist=%s\n", geninitfirstitem.c_str(), EXAM_TYPES[m_itemList[probeIndex][i]].c_str());

        if (geninitfirstitem == EXAM_TYPES[m_itemList[probeIndex][i]]) {
          m_itemIndex =(ExamItem::EItem)m_itemList[probeIndex][i];
        }
      }
    } else {
      m_itemIndex = indexItem;
    }
  } else {
    // keeping calc and bodymark are right
    ExamItem exam;
    string genfirstitem = exam.GetUserItemInfo(curPara.model);

    int probeIndex = ViewProbe::GetInstance()->GetProbeIndex();

    if (probeIndex < MAX_SOCKET) {
      for(int i = 0; i < (int)m_itemList[probeIndex].size(); i++) {
        PRINTF("str_firesrt_item=%s,  examlist=%s\n",genfirstitem.c_str(),EXAM_TYPES[m_itemList[probeIndex][i]].c_str());

        if (genfirstitem == EXAM_TYPES[m_itemList[probeIndex][i]]) {
          m_itemIndex =(ExamItem::EItem)m_itemList[probeIndex][i];
        }
      }
    } else {
      m_itemIndex = indexItem;
    }
  }

  m_socketIndex = indexSocket;

  // update
  AbsUpdate2D* m_ptrUpdate = GlobalClassMan::GetInstance()->GetUpdate2D();

  if (indexSocket == MAX_SOCKET) {
    m_ptrUpdate->ProbeType("");
  } else {
    m_ptrUpdate->ProbeType(curPara.model);
  }

  m_ptrUpdate->UserDefineExamItem(item.c_str());
  m_e.SetUserItemOfProbe(curPara.model, (ExamItem::EItem)m_itemIndex, item);
  ExamItem::ParaItem paraItem = m_e.GetCurrentItemPara();

  g_menuCalc.ChangeExamItem(item);
  g_menuMeasure.ChangeExamItem(item);

  // init 2D
  Img2D* ptrImg2D = Img2D::GetInstance();
  ptrImg2D->SetCalc2D( GlobalClassMan::GetInstance()->GetCalc2D(curPara.model) );
  ptrImg2D->InitProbe2D(&curPara, &paraItem);
  ImgProc2D::GetInstance()->Init(&(paraItem.d2));
  ptrImg2D->InitProbeM(&curPara, &paraItem);

  // init M
  ImgProcM::GetInstance()->Init(&(paraItem.d2));

  // init pw
  ImgPw* ptrImgPw = ImgPw::GetInstance();
  ptrImgPw->SetCalcPw( GlobalClassMan::GetInstance()->GetCalcPw(curPara.model) );
  ptrImgPw->InitProbe(&curPara, &paraItem);
  ImgProcPw::GetInstance()->Init(&(paraItem.spectrum));

  // init cfm
  ImgCfm* ptrImgCfm = ImgCfm::GetInstance();
  ptrImgCfm->SetCalcCfm( GlobalClassMan::GetInstance()->GetCalcCfm(curPara.model) );
  ptrImgCfm->InitProbe(&curPara, &paraItem);
  ImgProcCfm::GetInstance()->Init(&(paraItem.color));

  // recreate biopsy line
  BiopsyLine::GetInstance()->Create();

  // enter 2D scan mode
  ScanMode::GetInstance()->DarkAllModeLight();
  g_keyInterface.CtrlLight(TRUE,LIGHT_D2);

  ScanMode::GetInstance()->Enter2D();

  // power on HV
  m_ptrProbe->ActiveHV(TRUE);

  //enable emit
  Img2D::GetInstance()->EnableEmit();

  usleep(1000);

  Update2D::SetCineRemoveImg(3);
  // unfreeze
  FreezeMode* ptrFreeze = FreezeMode::GetInstance();
  ptrFreeze->UnFreeze();
}

bool ProbeSelect::OneProbeRead(int socket) {
  int select = MAX_SOCKET;

  // power off HV
  m_ptrProbe->ActiveHV(FALSE);

  // read probe
  m_ptrProbe->GetOneProbe(m_para, socket);

  // get exam item list
  if (m_para[socket].exist) {
    PRINTF("================get Probe Model: %s\n", m_para[socket].model);
    m_itemList[socket] = m_e.GetItemListOfProbe(m_para[socket].model);
    select = socket;
  } else {
    m_itemList[socket].clear();
  }

  if (select == MAX_SOCKET) {
    return false;
  } else {
    return true;
  }
}

void ProbeSelect::ProbeInit(int indexSocket, ExamItem::EItem indexItem) {
  // exit auto optimize if in it
  KeyAutoOptimize::AutoOff();

  // exit local zoom if in it
  KeyLocalZoom klz;
  klz.ExitLocalZoom();

  // begin init
  m_itemIndex = indexItem;
  m_socketIndex = indexSocket;

  // get real probe and item parameter
  m_ptrProbe->SetProbeSocket(indexSocket);

  ProbeSocket::ProbePara curPara;
  m_ptrProbe->GetCurProbe(curPara);

  // update
  AbsUpdate2D* m_ptrUpdate = GlobalClassMan::GetInstance()->GetUpdate2D();
  if (indexSocket == MAX_SOCKET) {
    m_ptrUpdate->ProbeType("");
  } else {
    m_ptrUpdate->ProbeType(curPara.model);
  }

  m_ptrUpdate->ExamItem(indexItem);

  m_e.SetItemOfProbe(curPara.model, (ExamItem::EItem)indexItem);
  ExamItem::ParaItem paraItem = m_e.GetCurrentItemPara();

  g_menuCalc.ChangeExamItem(m_e.ITEM_LIB[indexItem]);
  g_menuMeasure.ChangeExamItem(m_e.ITEM_LIB[indexItem]);

  // init 2D
  Img2D* ptrImg2D = Img2D::GetInstance();
  ptrImg2D->SetCalc2D( GlobalClassMan::GetInstance()->GetCalc2D(curPara.model) );
  ptrImg2D->InitProbe2D(&curPara, &paraItem);
  ImgProc2D::GetInstance()->Init(&(paraItem.d2));

  // init M
  ptrImg2D->InitProbeM(&curPara, &paraItem);
  ImgProcM::GetInstance()->Init(&(paraItem.d2));

  // init pw
  ImgPw* ptrImgPw = ImgPw::GetInstance();
  ptrImgPw->SetCalcPw( GlobalClassMan::GetInstance()->GetCalcPw(curPara.model) );
  ptrImgPw->InitProbe(&curPara, &paraItem);
  ImgProcPw::GetInstance()->Init(&(paraItem.spectrum));

  // init cfm
  ImgCfm* ptrImgCfm = ImgCfm::GetInstance();
  ptrImgCfm->SetCalcCfm( GlobalClassMan::GetInstance()->GetCalcCfm(curPara.model) );
  ptrImgCfm->InitProbe(&curPara, &paraItem);
  ImgProcCfm::GetInstance()->Init(&(paraItem.color));

  // recreate biopsy line
  BiopsyLine::GetInstance()->Create();

  // enter 2D scan mode
  ScanMode::GetInstance()->Enter2D();

  //test
  m_ptrProbe->ActiveHV(TRUE);

  //enable emit
  Img2D::GetInstance()->EnableEmit();

  usleep(1000);

  Update2D::SetCineRemoveImg(3);

  // unfreeze
  FreezeMode* ptrFreeze = FreezeMode::GetInstance();
  ptrFreeze->UnFreeze();
}

void ProbeSelect::GetDefaultValue(int& socket, ExamItem::EItem& item, bool& flag) {
  int i;
  for (i = 0; i < MAX_SOCKET; i ++) {
    if (m_para[i].exist == TRUE) {
      break;
    }
  }

  int defaultSocket = m_ptrProbe->GetDefaultProbeSocket();

  if (defaultSocket != MAX_SOCKET) {
    IniFile ini(string(CFG_RES_PATH) + string(STORE_DEFAULT_ITEM_PATH));
    item = (ExamItem::EItem)(m_e.ReadDefaultProbeDefaultItem(&ini));
    socket = defaultSocket;
  } else {
    socket = i;

    if (i == MAX_SOCKET) {
      // get default probe para
      ProbeSocket::ProbePara paraDefault;
      m_ptrProbe->GetDefaultProbe(paraDefault);
      // get default item
      item = m_e.GetDefaultItem(paraDefault.model);
    } else {
      item = m_e.GetDefaultItem(m_para[i].model);
    }

    flag = false;
  }
}

void ProbeSelect::GetDefaultItemNameandFlag(string& itemName, bool& flag) {
  IniFile ini(string(CFG_RES_PATH) + string(STORE_DEFAULT_ITEM_PATH));
  ExamItem exam;
  itemName = exam.ReadDefaultProbeDefaultItemName(&ini);
  flag = exam.ReadUserItemFlag(&ini);
}

void ProbeSelect::GetPara(ProbeSocket::ProbePara*& para, vector<ExamItem::EItem>*& item,int& maxSocket) {
  maxSocket = MAX_SOCKET;
  para  = m_para;
  item = m_itemList;
}

void ProbeSelect::ActiveHV(bool on) {
  m_ptrProbe->ActiveHV(on);
}
