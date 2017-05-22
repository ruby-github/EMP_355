#include "keyboard/KeyFunc.h"

#include "utils/FakeXUtils.h"
#include "utils/MessageDialog.h"

#include "calcPeople/MenuCalcNew.h"
#include "calcPeople/ViewReport.h"
#include "comment/Arrow.h"
#include "comment/NoteArea.h"
#include "display/HintArea.h"
#include "display/ImageArea.h"
#include "imageControl/Img2D.h"
#include "imageControl/ImgCfm.h"
#include "imageControl/ImgPw.h"
#include "imageProc/GlobalClassMan.h"
#include "imageProc/ImgProc2D.h"
#include "imageProc/ImgProcM.h"
#include "imageProc/ImgProcCfm.h"
#include "imageProc/ImgProcPw.h"
#include "imageProc/MenuReview.h"
#include "imageProc/ScanMode.h"
#include "imageProc/Zoom.h"
#include "keyboard/LightDef.h"
#include "measure/DrawHistogram.h"
#include "measure/MeasureFactory.h"
#include "measure/MeasureMan.h"
#include "measure/MenuMeasure.h"
#include "patient/ImgMan.h"
#include "patient/PatientInfo.h"
#include "patient/ViewNewPat.h"
#include "patient/ViewWorkList.h"
#include "periDevice/DCMMan.h"
#include "periDevice/PeripheralMan.h"
#include "probe/MenuBiopsy.h"
#include "sysMan/SysDicomSetting.h"
#include "sysMan/SysGeneralSetting.h"
#include "sysMan/SysOptions.h"
#include "ViewMain.h"

#define DEMO_PATH "/mnt/harddisk/demo"

extern bool g_calcPwStatus;

static ImgMan::ImgItem item;

int g_count11 = 0;

// ---------------------------------------------------------

static gboolean DelayToSaveSnap(gpointer data) {
  ScanMode::SpecialMeasurePara para;
  ScanMode::GetInstance()->GetSpecialMeasurePara(&para);
  item.para = para;
  ImageArea::GetInstance()->GetSnapData(&item.data, &item.width, &item.height);
  ImgMan::ImgItem item_frm;
  ImageArea::GetInstance()->GetFrmData(&item_frm.data, &item_frm.width, &item_frm.height);

  int format_bak = ImgMan::GetInstance()->GetImgFormat();

  // save to image manage
  if(ImgMan::GetInstance()->GetImgStorage() == ImgMan::UDISK) {
    PeripheralMan* ptrPeripheralMan = PeripheralMan::GetInstance();

    if(!ptrPeripheralMan->CheckUsbStorageState()) {
      HintArea::GetInstance()->UpdateHint(_("Error: No USB device Plug-in!"), 2);
      free(data);
      free(item.data);
      item.data = NULL;
      return ERROR;
    }

    if(!ptrPeripheralMan->MountUsbStorage()) {
      HintArea::GetInstance()->UpdateHint(_("Error: Failed to mount USB device!"), 2);
      free(data);
      free(item.data);
      item.data = NULL;
      return ERROR;
    }

    if(ImgMan::GetInstance()->SaveSnap(0, (char*)data, UDISK_PATH, &item) == 0) {
      ImgMan::GetInstance()->SetImgFormat(ImgMan::FRM);
      ImgMan::GetInstance()->SaveSnap(0, (char*)data, UDISK_PATH, &item_frm);
      ImgMan::GetInstance()->SetImgFormat(format_bak);
    }

    ptrPeripheralMan->UmountUsbStorage();
  } else { // save to harddisk
    if(ImgMan::GetInstance()->SaveSnap(0, (char*)data, STORE_PATH, &item) == 0) {
      ImgMan::GetInstance()->SetImgFormat(ImgMan::FRM);
      ImgMan::GetInstance()->SaveSnap(0, (char*)data, STORE_PATH, &item_frm);
      ImgMan::GetInstance()->SetImgFormat(format_bak);
    }
  }

  free(item.data);
  item.data = NULL;
  free(item_frm.data);
  free(data);;

  if (MenuArea::GetInstance()->GetMenuType() == MenuArea::REVIEW) {
    g_menuReview.UpdatePreviewList();
    g_menuReview.UpdateLabel();
  }

  return false;
}

static int SaveSnap(gpointer data) {
  HintArea::GetInstance()->UpdateHint(_("Saving..."), 0);

  if (MenuArea::GetInstance()->GetMenuType() == MenuArea::REVIEW) {
    g_menuReview.HideTooltips();
  }

  g_timeout_add(100, DelayToSaveSnap, data);

  return OK;
}

void PowerOff(gpointer data) {
  execlp("poweroff", "poweroff", NULL);
  perror("PowerOff Failed");
}

int OpenNewPatWindow(gpointer data) {
  ViewNewPat::GetInstance()->CreateWindow();
  return 0;
}

int EndExam(gpointer data) {
  PRINTF("begin Exam\n");
  string errmsg;

  // archive patient info
  if (g_patientInfo.ArchivePatientInfo(errmsg)) {
    ViewNewPat::GetInstance()->UpdateStudyInfo();
    g_menuCalc.ClearAllData();
    g_menuMeasure.ClearAllData();
    // archive and clear image and viedo
    //g_patientInfo.ArchiveImg();
    // archive report
    ViewReport::GetInstance()->LoadMeasureDataForArchive();
    g_patientInfo.ArchiveReport();
    // archive and clear image and viedo
    g_patientInfo.ArchiveImg();
    // clear patient info
    g_patientInfo.ClearAll();
    // clear desctiption and comment
    ViewReport::GetInstance()->ClearIndicationandComments();
    //clear screen
    KeyClearScreen kcs;
    kcs.Execute();

    // update review menu
    MenuArea* ptrMenu = MenuArea::GetInstance();
    if (ptrMenu->GetMenuType() == MenuArea::REVIEW) {
      ptrMenu->ShowReviewMenu();
    }

    if (GTK_IS_WIDGET(ViewNewPat::GetInstance()->GetWindow())) {
      if (ViewNewPat::GetInstance()->GetClearStatus())
        ViewNewPat::GetInstance()->ClearData();
      else
        ViewNewPat::GetInstance()->ClearExamData();
    }

    setlocale(LC_NUMERIC, "en_US.UTF-8");

    if(CDCMMan::GetMe()->EndStudy()) {
      SysDicomSetting sysDicomSetting;

      if(sysDicomSetting.GetMPPS()) {
          if(ViewNewPat::GetInstance()->GetMPPSFlag()) {
            int Year, Month, Day, Hour, Minute, Second;
            GetCurrentDateTime(Year, Month, Day, Hour, Minute, Second);

            string year, mon, day, studyEndDate;
            ChangeDateFormatToString(Year, Month, Day, year, mon, day);
            studyEndDate = year+mon+day;

            string hour, min, sec, studyEndTime;
            ChangeTimeFormatToString(Hour, Minute, Second, hour, min, sec);
            studyEndTime = hour + min + sec;

            CDCMMan::GetMe()->EndMPPS(studyEndDate,studyEndTime);
            ViewNewPat::GetInstance()->SetMPPSFlag(false);
          }
        }

        SysGeneralSetting sysGS;
        int language= sysGS.GetLanguage();
        if (ZH == language) {
            setlocale(LC_NUMERIC, "zh_CN.UTF-8");
        } else if (RU == language) {
            setlocale(LC_NUMERIC, "ru_RU.UTF-8");
        } else if (PL == language) {
            setlocale(LC_NUMERIC, "pl_PL.UTF-8");
        } else if (FR == language) {
            setlocale(LC_NUMERIC, "fr_FR.UTF-8");
        } else if (DE == language) {
            setlocale(LC_NUMERIC, "de_DE.UTF-8");
        } else if(ES == language) {
            setlocale(LC_NUMERIC, "es_ES.UTF-8");
        } else {
            setlocale(LC_NUMERIC, "en_US.UTF-8");
        }

        ViewWorkList::GetInstance()->ClearQueryRes();
        ViewNewPat::GetInstance()->ClearStudyInfo();
        PRINTF("End Exam\n");
    } else {
      HintArea::GetInstance()->UpdateHint(_("[End Exam]: All details of patient has been archived."), 1);
      PRINTF("End Exam\n");
    }
  } else {
    if (GTK_IS_WIDGET(ViewNewPat::GetInstance()->GetWindow())) {
      MessageDialog::GetInstance()->Create(GTK_WINDOW(ViewNewPat::GetInstance()->GetWindow()), MessageDialog::DLG_ERROR, _(errmsg.c_str()), NULL);
    } else {
      MessageDialog::GetInstance()->Create(GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()), MessageDialog::DLG_ERROR, _(errmsg.c_str()), OpenNewPatWindow, OpenNewPatWindow);
    }
  }

  return 0;
}

void ChangeTis() {
  ModeStatus s;
  int curImg = s.GetPwCurImg();

  if (ModeStatus::IsSpectrumImgMode()) {
    if (curImg == 2)
      ImgPw::GetInstance()->ChangePwTis();
    else if (curImg == 1)
      Img2D::GetInstance()->Change2DTis();
  } else if (ModeStatus::IsColorImgMode()) {
    ImgCfm::GetInstance()->ChangeCfmTis();
  } else if (ModeStatus::IsSpectrumColorImgMode()) {
    if (curImg == 2) //PW
      ImgPw::GetInstance()->ChangePwTis();
    else if (curImg == 1)
      ImgCfm::GetInstance()->ChangeCfmTis();
  } else {
    Img2D::GetInstance()->Change2DTis();
  }
}

void DarkFucusLight() {
}

//////////////////////// KeySwitchIM ///////////////////////

bool KeySwitchIM::m_imOn = false;

bool KeySwitchIM::Execute() {
  PRINTF("Switch Input Method\n");
  doKeyEvent(37, 1);
  doKeyEvent(65, 1);
  doKeyEvent(65, 0);
  doKeyEvent(37, 0);

  if(m_imOn) {
    m_imOn = false;
  } else {
    m_imOn = true;
  }

  return true;
}

bool KeySwitchIM::ExcuteChange(bool issitch) {
  if (issitch) {
    doKeyEvent(37, 1);
    doKeyEvent(65, 1);
    doKeyEvent(65, 0);
    doKeyEvent(37, 0);

    if(m_imOn) {
      m_imOn = false;
    } else {
      m_imOn = true;
    }
  }

  return true;
}

//////////////////////// KeyPowerOff ///////////////////////

bool KeyPowerOff::Execute() {
  FpgaCtrl2D ctrl2D;
  ctrl2D.SendPowerOff();
  PowerOff(NULL);

  return true;
}

/////////////////////////// KeyTSI /////////////////////////

int KeyTSI::keyNum = 0;

bool KeyTSI::Execute() {
  Img2D::GetInstance()->ChangeTSI(EKnobOper(ROTATE));
  return true;
}

bool KeyTSI::Do() {
  Img2D::GetInstance()->ChangeTSI(EKnobOper(ROTATE));
  return true;
}

bool KeyTSI::Undo() {
  MultiFuncUndo();
  return true;
}

////////////////////// KeyAutoOptimize /////////////////////

bool KeyAutoOptimize::m_autoOn = false;

ExamItem::ParaItem KeyAutoOptimize::m_itemPara;

bool KeyAutoOptimize::Execute() {
  if(m_autoOn == false) {
    m_autoOn = true;
    g_keyInterface.CtrlLight(m_autoOn, LIGHT_AUTO);
    Do();
  } else {
    m_autoOn = false;
    g_keyInterface.CtrlLight(m_autoOn, LIGHT_AUTO);
    Undo();
  }

  Img2D::GetInstance()->UpdateAutoOptimize(m_autoOn);

  return true;
}

void KeyAutoOptimize::AutoOff() {
  if (m_autoOn) {
    m_autoOn = FALSE;
    Img2D::GetInstance()->UpdateAutoOptimize(m_autoOn);
  }
}

bool KeyAutoOptimize::Do() {
  // bakup current parameter
  BackupPara();

  // apply auto optimize
  ProbeMan* ptrMan = ProbeMan::GetInstance();
  ProbeSocket::ProbePara pPara;
  ptrMan->GetCurProbe(pPara);

  ExamItem e;
  ExamItem::ParaItem ePara = e.GetImgOptimize(pPara.model);
  ImgOptimize(pPara, ePara);

  return true;
}

bool KeyAutoOptimize::Undo() {
  // back to bakup parameter
  ProbeMan* ptrMan = ProbeMan::GetInstance();
  ProbeSocket::ProbePara pPara;
  ptrMan->GetCurProbe(pPara);

  ImgOptimize(pPara, m_itemPara);

  return true;
}

void KeyAutoOptimize::ImgOptimize(ProbeSocket::ProbePara p, ExamItem::ParaItem i) {
  ModeStatus ms;
  ScanMode::EScanMode mode = ms.GetScanMode();

  printf("--%s--\n", __FUNCTION__);
  //exit simult

  if (ImgPw::GetInstance()->GetSimut2Status()) {
    ScanMode::GetInstance()->EnterPwSimult(false);
  }

  if (ImgPw::GetInstance()->GetSimut3Status()) {
    if (mode == ScanMode::PWPDI_SIMULT) {
      ScanMode::GetInstance()->EnterPwPdiSimult(false);
    } else {
      ScanMode::GetInstance()->EnterPwCfmSimult(false);
    }
  }

  usleep(500000);

  // init 2D
  Img2D* ptrImg2D = Img2D::GetInstance();
  ptrImg2D->InitProbe2DOptimize(&p, &i);
  ImgProc2D::GetInstance()->InitOptimize(&(i.d2));

  // init M
  ptrImg2D->InitProbeMOptimize(&p, &i);
  ImgProcM::GetInstance()->InitOptimize(&(i.d2));

  // init pw
  ImgPw* ptrImgPw = ImgPw::GetInstance();
  ptrImgPw->InitProbeOptimize(&p, &i);
  ImgProcPw::GetInstance()->InitOptimize(&(i.spectrum));

  // init cfm
  ImgCfm* ptrImgCfm = ImgCfm::GetInstance();
  ptrImgCfm->InitProbeOptimize(&p, &i);
  ImgProcCfm::GetInstance()->InitOptimize(&(i.color));

  // 如果是M模式，则需要重新计算M增益，因为在PW InitProbeOptimize中PW增益覆盖了M增益
  if (ModeStatus::IsMImgMode()) {
    ptrImg2D->ReSendGainM();
  }

  //clear screen
  KeyClearScreen kcs;
  kcs.Execute();
  kcs.ModeExecute(true);

  // update img when need
  int curPw = ms.GetPwCurImg();

  if ((mode == ScanMode::PW) && (curPw == 2)) {
    ScanMode::GetInstance()->SwitchPw();
    usleep(500000);
    ScanMode::GetInstance()->SwitchPw();
  } else if ((mode == ScanMode::PWCFM) && (curPw == 2)) {
    ScanMode::GetInstance()->SwitchPwCfm();
    usleep(500000);
    ScanMode::GetInstance()->SwitchPwCfm();
  } else if ((mode == ScanMode::PWPDI) && (curPw == 2)) {
    ScanMode::GetInstance()->SwitchPwPdi();
    usleep(500000);
    ScanMode::GetInstance()->SwitchPwPdi();
  }

  if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::LOCAL_ZOOM && !Zoom::GetInstance()->GetLocalZoomStatus()) {
    ImageAreaDraw::GetInstance()->ReDrawLocalZoom();
  }

  if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::PIP_ZOOM) {
    ImageAreaDraw::GetInstance()->ReDrawMagnifier();
    ImageAreaDraw::GetInstance()->ReDrawPIPBox();
  }
}

void KeyAutoOptimize::BackupPara() {
  Img2D::GetInstance()->GetCurPara(&m_itemPara);
  ImgProc2D::GetInstance()->GetCurPara(&m_itemPara);

  ImgPw::GetInstance()->GetCurPara(&m_itemPara);
  ImgCfm::GetInstance()->GetCurPara(&m_itemPara);

  ImgProcPw::GetInstance()->GetCurPara(&m_itemPara);
  ImgProcCfm::GetInstance()->GetCurPara(&m_itemPara);
}

///////////////////////// KeyMeasure ///////////////////////

bool KeyMeasure::Execute() {
  AbsUpdateMix* m_ptrUpdate = GlobalClassMan::GetInstance()->GetUpdateMix();

  // printf("in the Measure which type is :%s\n",MenuArea::GetInstance()->GetMenuType());

  if (MenuArea::GetInstance()->GetMenuType() == MenuArea::MEASURE) {
    if(g_ptrAbsMeasure != NULL) {
      delete g_ptrAbsMeasure;
      g_ptrAbsMeasure = NULL;
    }

    m_ptrUpdate->ExitMeasure();
    MultiFuncUndo();
  } else {
    if (g_menuReview.GetLimit() == 2) { //note: pic read from external media can not measure
      HintArea::GetInstance()->UpdateHint(_("[Measure]: Measure is disable in current image."), 2);
    } else {
      PRINTF("Enter measure\n");
      MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::NONE);

      if(g_ptrAbsMeasure != NULL) {
        delete g_ptrAbsMeasure;
        g_ptrAbsMeasure = NULL;
      }

      //SetChangePointerMeasure();
      m_ptrUpdate->EnterMeasure();
    }
  }

  return true;
}

////////////////////////// KeyCalc /////////////////////////

bool KeyCalc::Execute() {
  AbsUpdateMix* m_ptrUpdate = GlobalClassMan::GetInstance()->GetUpdateMix();

  if (MenuArea::GetInstance()->GetMenuType() == MenuArea::CALC) {
    if(g_ptrAbsMeasure != NULL) {
      delete g_ptrAbsMeasure;
      g_ptrAbsMeasure = NULL;
    }

    m_ptrUpdate->ExitCalc();
    MultiFuncUndo();
  } else {
    if (g_menuReview.GetLimit() != 0) { //note: pic read from archived and usb media can not calc
      HintArea::GetInstance()->UpdateHint(_("[Calc]: Calc is disable in current image."), 2);
    } else {
      MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::NONE);

      if(g_ptrAbsMeasure !=NULL) {
        delete g_ptrAbsMeasure;
        g_ptrAbsMeasure = NULL;
      }

      m_ptrUpdate->EnterCalc();
    }
  }

  return true;
}

///////////////////////// KeySystem ////////////////////////

bool KeySystem::Execute() {
  AbsUpdateMix* m_ptrUpdate = GlobalClassMan::GetInstance()->GetUpdateMix();
  MultiFuncFactory* ptr = MultiFuncFactory::GetInstance();

  if (MenuArea::GetInstance()->GetMenuType() == MenuArea::SYSTEM) {
    delete g_ptrAbsMeasure;
    g_ptrAbsMeasure = NULL;
    m_ptrUpdate->ExitSystem();
    MultiFuncUndo();
  } else {
    delete g_ptrAbsMeasure;
    g_ptrAbsMeasure = NULL;
    m_ptrUpdate->EnterSystem();
    ptr->Create(MultiFuncFactory::NONE);
  }

  return true;
}

///////////////////////// KeyCursor ////////////////////////

bool KeyCursor::Execute() {
  MultiFuncFactory* ptr = MultiFuncFactory::GetInstance();
  MultiFuncFactory::EMultiFunc type = ptr->GetMultiFuncType();

  if ((type != MultiFuncFactory::EFOV) && (type != MultiFuncFactory::NONE)) {
    if(type== MultiFuncFactory::BIOPSY_VERIFY) {
      ptr->Create(MultiFuncFactory::BIOPSY);
      SetSystemCursor(90,130);
      doBtnEvent(1, 1);
      doBtnEvent(1, 0);
      SetMenuBiopsyCursorYRange(128,213);
    } else {
      if(type== MultiFuncFactory::BIOPSY) {
        if(MenuArea::GetInstance()->GetMenuType()==MenuArea::BIOPSYVERIFY) {
          SetSystemCursor(90,155);
          doBtnEvent(1, 1);
          doBtnEvent(1, 0);
        }

        MultiFuncUndo();
      } else {
        ptr->Create(MultiFuncFactory::NONE);
      }
    }
  } else {
    MultiFuncUndo();
  }

  return true;
}

////////////////////////// KeyText /////////////////////////

bool KeyText::Execute() {
  MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::TEXT);
  return true;
}

////////////////////////// KeyArrow ////////////////////////

bool KeyArrow::Execute() {
  if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::ARROW)
    MultiFuncUndo();
  else {
    HintArea::GetInstance()->UpdateHint(_("[Arrow]: <Auto> to change arrow direction."), 2);
    MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::ARROW);
  }

  return true;
}

///////////////////////// KeyBiopsy ////////////////////////

bool KeyBiopsy::Execute() {
  if ((MenuArea::GetInstance()->GetMenuType() == MenuArea::BIOPSY)||(MenuArea::GetInstance()->GetMenuType() == MenuArea::BIOPSYBRACKET)||(MenuArea::GetInstance()->GetMenuType() == MenuArea::BIOPSYVERIFY)) {
    MenuShowUndo();
    MultiFuncUndo();
  } else {
    ModeStatus ms;

    if ((((ms.GetScanMode()==ScanMode::D2) && (ms.GetFormat2D() == Format2D::B))||((ms.GetScanMode()==ScanMode::CFM) &&(ms.GetFormatCfm()==FormatCfm::B))||((ms.GetScanMode()==ScanMode::PDI) &&(ms.GetFormatCfm()==FormatCfm::B)))&& ms.IsUnFreezeMode()) {
      MenuArea::GetInstance()->ShowBiopsyMenu();
      MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::BIOPSY);
    } else {
      HintArea::GetInstance()->UpdateHint(_("[Biopsy]: Only valid in B mode,CFM,PDI and UnFreeze status."), 1);
    }
  }

  return true;
}

//////////////////////// KeyBodyMark ///////////////////////

bool KeyBodyMark::Execute() {
  MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::BODYMARK);
  return true;
}

//////////////////////// KeySaveSnap ///////////////////////

bool KeySaveSnap::Execute() {
  if(item.data) {
    return true;
  }

  SysOptions so;

  if(!so.GetImageAutoName()) {
    if(!ModeStatus::IsFreezeMode()) {
      FreezeMode::GetInstance()->PressFreeze();
    }

    MessageDialog::GetInstance()->Create(GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()),
      MessageDialog::DLG_FILENAME, _("Please input the file name(without suffix)"), &SaveSnap);
  } else {
    // generate file name automaticly
    time_t at;
    at = time(&at);

    struct tm* ct;
    ct = localtime(&at);

    char *fileName = (char*)malloc(256);
    memset(fileName, 0, 15);
    sprintf(fileName, "%d%d%d%d%d%d%d%d%d%d%d", (ct->tm_year+1900), (ct->tm_mon+1)/10, (ct->tm_mon+1)%10, (ct->tm_mday)/10, (ct->tm_mday)%10, (ct->tm_hour)/10, (ct->tm_hour)%10, (ct->tm_min)/10, (ct->tm_min)%10, (ct->tm_sec)/10, (ct->tm_sec)%10);

    SaveSnap(fileName);
  }

  return true;
}

///////////////////////// KeyReview ////////////////////////

bool KeyReview::m_menuRead = false;

bool KeyReview::Execute() {
  MenuArea* ptrMenu = MenuArea::GetInstance();
  MultiFuncFactory* ptrFunc = MultiFuncFactory::GetInstance();

  if (m_menuRead) {
    //FreezeMode::GetInstance()->PressUnFreeze();
    MenuShowUndo();
    m_menuRead = FALSE;
  } else {
    // freeze
    if (!ModeStatus::IsFreezeMode()) {
      FreezeMode::GetInstance()->PressFreeze();
    }

    ptrMenu->ShowReviewMenu();
    ptrFunc->Create(MultiFuncFactory::NONE);
    m_menuRead = true; // must after ShowReviewMenu(HideAllMenu), be care of "HideMenuReview"
  }

  return true;
}

void KeyReview::HideMenuReview() {
  if (m_menuRead) {
    m_menuRead = false;
  }
}

void KeyReview::ExitMenuReivew() {
  if (m_menuRead) {
    MenuShowUndo();
    m_menuRead = false;

    FreezeMode::GetInstance()->PressUnFreeze();
  }
}

void KeyReview::SetDefaultIDAndPath() {
  // set menureview's file path and folder name
  if (!m_menuRead) {
    PatientInfo::Info info;
    g_patientInfo.GetInfo(info);
    g_menuReview.SetLimit(0);
    g_menuReview.SetPatientID(0);
    g_menuReview.SetImgPath((const char*)STORE_PATH);
  }
}

void KeyReview::SetDemoIDAndPath() {
  // set menureview's file path and folder name
  if (!m_menuRead) {
    PatientInfo::Info info;
    g_patientInfo.GetInfo(info);
    g_menuReview.SetLimit(0);
    g_menuReview.SetPatientID(0);
    g_menuReview.SetImgPath((char*) DEMO_PATH);
  }
}

/////////////////////////// KeyPIP /////////////////////////

bool KeyPIP::Execute() {
  if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::PIP_ZOOM) {
    MultiFuncUndo();
  } else {
    ModeStatus ms;
    if (ms.IsD2Mode() && (ms.GetFormat2D() == Format2D::B) && ms.IsUnFreezeMode()) {
      MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::PIP_ZOOM);
    } else {
      HintArea::GetInstance()->UpdateHint(_("[PIP]: Only valid in B mode and UnFreeze status."), 1);
    }
  }

  return true;
}

//////////////////////// KeyLocalZoom //////////////////////

bool KeyLocalZoom::Execute() {
  if (ExitLocalZoom()) {
    return true;
  }

  if(MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::LOCAL_ZOOM) {
    MultiFuncUndo();
    Zoom::GetInstance()->LocalZoomCtrl(FALSE);
  } else {
    ModeStatus ms;
    SysOptions sysOption;

    bool inSnap = Replay::GetInstance()->GetReadSnapStatus();
    if (inSnap) {
      HintArea::GetInstance()->UpdateHint(_("[Local Zoom]: Invalid when in status read picture."), 1);
      return true;
    }

    if (ms.IsD2Mode() && (ms.GetFormat2D() == Format2D::B) && (ms.IsUnFreezeMode() || ms.IsPureFreezeMode())) {
      if ((Img2D::GetInstance()->GetTpViewStatus()) || (g_menuBiopsy.GetDrawStatus()) || (!sysOption.GetCenterLine())) {
        HintArea::GetInstance()->UpdateHint(_("[Local Zoom]: Invalid when Tp-view, biopsy line or centerline is on."), 1);
      } else {
        int steer = Img2D::GetInstance()->GetSteer();

        if (steer == 0) {
          int rotate = ImgProc2D::GetInstance()->GetRotate();

          if (rotate == 0) {
            MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::LOCAL_ZOOM);
          } else {
            HintArea::GetInstance()->UpdateHint(_("[Local Zoom]: Invalid when image is rotated."), 1);
          }
        } else {
          HintArea::GetInstance()->UpdateHint(_("[Local Zoom]: Invalid when image is steered."), 1);
        }
      }
    } else {
      HintArea::GetInstance()->UpdateHint(_("[Local Zoom]: Only valid in 2D B mode and not replay status."), 1);
    }
  }

  return true;
}

bool KeyLocalZoom::ExitLocalZoom() {
  ModeStatus s;
  int flag = 0;

  if (Zoom::GetInstance()->GetLocalZoomStatus()) {
    if(MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::LOCAL_ZOOM) {
      MultiFuncUndo();
    }

    // judge mode
    if (s.IsColorMode())
      flag = 1;
    else if (s.IsSpectrumMode())
      flag = 2;
    else if (s.IsSpectrumColorMode())
      flag = 3;
    else if (s.IsMMode())
      flag = 4;

    // clear line according to mode
    if ((flag == 1) || (flag == 3))
      ImgCfm::GetInstance()->ClearBox();
    if ((flag == 2) || (flag == 3))
      ImgPw::GetInstance()->ClearSv();
    else if (flag == 4)
      Img2D::GetInstance()->ClearMLine();

    // clear biopsy line
    g_menuBiopsy.ClearBiopsyLine();

    Zoom::GetInstance()->LocalZoomCtrl(FALSE);

    //redraw biopsyline
    g_menuBiopsy.UpdateBiopsyLine();

    // redraw line according to mode
    if ((flag == 1) || (flag == 3))
      ImgCfm::GetInstance()->ReDrawBox();
    if ((flag == 2) || (flag == 3))
      ImgPw::GetInstance()->ReDrawSv();
    else if (flag == 4)
      Img2D::GetInstance()->ReDrawMLine();

    HintArea::GetInstance()->UpdateHint(_("[Local Zoom]: Exit."), 1);
    return true;
  }

  return false;
}

///////////////////////// KeyFocus /////////////////////////

bool KeyFocus::Execute() {
  if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::FOCUS) {
    MultiFuncUndo();
  } else {
    ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();
    if ((mode == ScanMode::PW_SIMULT) || (mode == ScanMode::PWCFM_SIMULT) || (mode == ScanMode::PWPDI_SIMULT) || (mode == ScanMode::EFOV)) {
      HintArea::GetInstance()->UpdateHint(_("[Focus]: Invalid when simult is on."), 1);
    } else {
      MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::FOCUS);
    }
  }

  return true;
}

///////////////////////// KeyDepth /////////////////////////

bool KeyDepth::ExecuteAdd() {
  DepthExe(ADD);
  return true;
}

bool KeyDepth::ExecuteSub() {
  DepthExe(SUB);
  return true;
}

void KeyDepth::DepthExe(EKnobOper oper) {
  ModeStatus s;
  int flag = 0;
  ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();

  if (s.IsColorMode())
    flag = 1;
  else if (s.IsSpectrumMode())
    flag = 2;
  else if (s.IsSpectrumColorMode())
    flag = 3;
  else if (s.IsMMode())
    flag = 4;

  if ((flag == 1) || (flag == 3))
    ImgCfm::GetInstance()->ClearBox();
  if ((flag == 2) || (flag == 3))
    ImgPw::GetInstance()->ClearSv();
  else if (flag == 4)
    Img2D::GetInstance()->ClearMLine();

  if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::LOCAL_ZOOM) {
    Zoom::GetInstance()->ClearLocalZoom();
  }

  // clear biopsy line
  g_menuBiopsy.ClearBiopsyLine();
  //prepare
  ImgCfm::GetInstance()->ChangeDepthBefore();

  //apply
  if (Img2D::GetInstance()->ChangeDepth(oper) != ERROR) {
    ImgCfm::GetInstance()->ChangeDepth();
    ImgPw::GetInstance()->ChangeDepth();
  }

  if ((flag == 1) || (flag == 3))
    ImgCfm::GetInstance()->ReDrawBox();
  if ((flag == 2) || (flag == 3))
    ImgPw::GetInstance()->ReDrawSv();
  else if (flag == 4)
    Img2D::GetInstance()->ReDrawMLine();

  if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::LOCAL_ZOOM) {
    Zoom::GetInstance()->RedrawLocalZoom();
  }

  // redraw biopsy line
  g_menuBiopsy.UpdateBiopsyLine();

  // change Tis
  ChangeTis();
}

bool KeyDepth::Execute() {
  if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::DEPTH) {
      MultiFuncUndo();
  } else {
    MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::DEPTH);
  }

  return true;
}

////////////////////////// KeyFreq /////////////////////////

bool KeyFreq::Execute() {
    if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::FREQ) {
      MultiFuncUndo();
    } else {
      ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();
      if ((mode == ScanMode::PW_SIMULT) || (mode == ScanMode::PWCFM_SIMULT) || (mode == ScanMode::PWPDI_SIMULT)) {
        HintArea::GetInstance()->UpdateHint(_("[Freq]: Invalid when simult is on."), 1);
      } else {
        MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::FREQ);
      }
    }

  return true;
}

///////////////////////// KeyChroma ////////////////////////

bool KeyChroma::Execute() {
  if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::CHROMA) {
    MultiFuncUndo();
  } else {
    MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::CHROMA);
  }

  return true;
}

////////////////////// KeyClearScreen //////////////////////

bool KeyClearScreen::Execute() {
  DeleteDataForClearScreen();
  // draw centerline
  KeyCenterLine key;
  //key.Clear();

  DrawHistogram::GetInstance()->SetOnOff(0);
  if ((MenuArea::GetInstance()->GetMenuType()==MenuArea::BIOPSY)||(MenuArea::GetInstance()->GetMenuType()==MenuArea::BIOPSYBRACKET)||(MenuArea::GetInstance()->GetMenuType()==MenuArea::BIOPSYVERIFY)) {
    MenuShowUndo();//2016.11.07--solve biopsy menu don't exit after biopsy line erased or biopsy line appeared when biopsy menu exited
    MultiFuncUndo();//2016.11.07--solve problem that biopsy verify don't exit.
  }

  ImageArea::GetInstance()->ClearScreen();

  if (Replay::GetInstance()->GetReadSnapStatus()) {
    ImageArea::GetInstance()->ResetReadImg();
    return true;
  }

  key.Execute();

  // Biopsy line
  g_menuBiopsy.UpdateBiopsyLine();

  // if or not freeze
  if (!ModeStatus::IsUnFreezeMode()) {
    Replay::GetInstance()->DisplayReplayBar();
  }

  return true;
}

bool KeyClearScreen::UnFreezeExecute(bool update) {
  // draw centerline
  KeyCenterLine key;
  //key.Clear();
  DrawHistogram::GetInstance()->SetOnOff(0);
  ImageArea::GetInstance()->ClearScreenUnFreeze();

  if (Replay::GetInstance()->GetReadSnapStatus()) {
    //ImageArea::GetInstance()->ResetReadImg();
    return true;
  }

  key.Execute();

  // Biopsy line
  g_menuBiopsy.UpdateBiopsyLine();

  // if or not freeze
  if (!ModeStatus::IsUnFreezeMode()) {
    Replay::GetInstance()->DisplayReplayBar();
  }

  return true;
}

bool KeyClearScreen::ModeExecute(bool update) {
  ModeStatus s;
  ScanMode::EScanMode mode = s.GetScanMode();

  //ImageArea::GetInstance()->ClearArea();
  DeleteDataForClearScreen();

  AbsUpdate2D* ptrUpdate2D = GlobalClassMan::GetInstance()->GetUpdate2D();
  AbsUpdatePw* ptrUpdatePw = GlobalClassMan::GetInstance()->GetUpdatePw();
  AbsUpdateCfm* ptrUpdateCfm = GlobalClassMan::GetInstance()->GetUpdateCfm();

  switch(mode) {
  case ScanMode::D2:
    ptrUpdate2D->Enter2DImg(update);
    break;
  case ScanMode::M_INIT:
    {
      ptrUpdate2D->Enter2DImg();
      ptrUpdate2D->EnterMImg();
    }

    break;
  case ScanMode::M:
    ptrUpdate2D->UpdateMImg();
    break;
  case ScanMode::CW_INIT:
  case ScanMode::PW_INIT:
    {
      ptrUpdate2D->Enter2DImg();
      ptrUpdatePw->EnterPwImg();
    }

    break;
  case ScanMode::PW:
  case ScanMode::PW_SIMULT:
  case ScanMode::CW:
    ptrUpdatePw->UpdatePwImg();
    break;
  case ScanMode::PWCFM_INIT:
  case ScanMode::CWCFM_INIT:
    {
      ptrUpdateCfm->EnterCfmImg();
      ptrUpdatePw->EnterPwImg();
    }

    break;
  case ScanMode::PWPDI_INIT:
  case ScanMode::CWPDI_INIT:
    {
      ptrUpdateCfm->EnterCfmImg();
      ptrUpdatePw->EnterPwImg();
    }

    break;
  case ScanMode::PWCFM:
  case ScanMode::PWCFM_SIMULT:
  case ScanMode::PWPDI:
  case ScanMode::PWPDI_SIMULT:
  case ScanMode::CWCFM:
  case ScanMode::CWPDI:
    ptrUpdatePw->UpdatePwCfmImg();
    break;
  case ScanMode::CFM:
  case ScanMode::PDI:
    ptrUpdateCfm->EnterCfmImg(update);
    break;
  case ScanMode::CFM_VS_2D:
  case ScanMode::PDI_VS_2D:
    ptrUpdateCfm->EnterCfmVs2D();
    break;
  case ScanMode::ANATOMIC_M:
    ptrUpdate2D->EnterAnatomicM();
    break;
  case ScanMode::EFOV:
    break;
  }

  return true;
}

void KeyClearScreen::DeleteDataForClearScreen() {
  // clear arrow data
  CArrow::DeleteAllForFreeze();

  // clear text data
  NoteArea::GetInstance()->Hide();
  ImageAreaDraw::GetInstance()->ClearMagnifier();

  // clear measure
  MeasureMan::GetInstance()->DeleteAllForClearScreen();

  if (g_ptrAbsMeasure != NULL) {
    delete g_ptrAbsMeasure;
    g_ptrAbsMeasure = NULL;
    //MultiFuncUndo();
    ModeStatus ms;
    ScanMode::EScanMode mode = ms.GetScanMode();

    if(mode != ScanMode::EFOV) {
      MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::NONE);
    } else {
      MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::EFOV);
    }
  }
}

//////////////////////// KeyEndExam ////////////////////////

bool KeyEndExam::Execute() {
  const char* info = N_("End Exam?");

  MessageDialog::GetInstance()->Create(GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()), MessageDialog::DLG_QUESTION, _(info), EndExam);
  PRINTF("end end exam execute\n");

  return true;
}

/////////////////////// KeyCenterLine //////////////////////

bool KeyCenterLine::Execute() {
  ModeStatus ms;
  if (((ms.GetScanMode() == ScanMode::D2) && (ms.GetFormat2D() == Format2D::B))
    || (((ms.GetScanMode() == ScanMode::CFM) || (ms.GetScanMode() == ScanMode::PDI)) && (ms.GetFormatCfm() == FormatCfm::B))
    || (ms.GetScanMode() == ScanMode::PWCFM_INIT) //solve that the center line is cleaned int pwcmf_init mode. lhm
    || (ms.GetScanMode() == ScanMode::PW_INIT)) { //solve that the center line is cleaned int pw_init mode. lhm
    SysOptions sys;
    ImageAreaDraw::GetInstance()->ClearCenterLine();

    if (sys.GetCenterLine() == 0) {
      ImageAreaDraw::GetInstance()->DrawCenterLine();
    } else {
      ImageAreaDraw::GetInstance()->ClearCenterLine();
    }
  }

  return true;
}

void KeyCenterLine::Clear() {
  return ;
}

///////////////////////// KeyMenu //////////////////////////

bool KeyMenu::Execute() {
  if (MenuArea::GetInstance()->GetMenuVisible())
    MenuArea::GetInstance()->HideMenu();
  else
    MenuArea::GetInstance()->ShowMenu();

  return true;
}
