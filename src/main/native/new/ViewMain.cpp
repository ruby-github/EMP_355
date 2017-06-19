#include "ViewMain.h"

#include "calcPeople/ViewReport.h"
#include "comment/NoteArea.h"
#include "display/HintArea.h"
#include "display/ImageArea.h"
#include "display/MenuArea.h"
#include "display/KnobMenu.h"
#include "display/TopArea.h"
#include "display/ViewIcon.h"
#include "imageControl/Knob2D.h"
#include "imageProc/Format2D.h"
#include "imageProc/FormatCfm.h"
#include "imageProc/FormatM.h"
#include "imageProc/FormatPw.h"
#include "imageProc/FreezeMode.h"
#include "imageProc/GlobalClassMan.h"
#include "imageProc/ImgProc2D.h"
#include "imageProc/ImgProcPw.h"
#include "imageProc/MenuPW.h"
#include "imageProc/ModeStatus.h"
#include "imageProc/ScanMode.h"
#include "imageProc/Zoom.h"
#include "keyboard/KeyDef.h"
#include "keyboard/KeyFunc.h"
#include "keyboard/KeyValueOpr.h"
#include "keyboard/LightDef.h"
#include "keyboard/MultiFuncFactory.h"
#include "measure/MeasureMan.h"
#include "patient/ViewArchive.h"
#include "patient/ViewNewPat.h"
#include "periDevice/MonitorControl.h"
#include "probe/ProbeMan.h"
#include "probe/ViewProbe.h"
#include "sysMan/SysGeneralSetting.h"
#include "sysMan/UserDefineKey.h"
#include "sysMan/UserSelect.h"
#include "sysMan/ViewDicomSupervise.h"
#include "sysMan/ViewSuperuser.h"
#include "sysMan/ViewSystem.h"

#include "utils/FakeXUtils.h"
#include "utils/MainWindowConfig.h"

#include <EmpAuthorization.h>

extern bool g_authorizationOn;

int g_curOper = 1;  // current operation (1: start line of cycle, 2: ps positoin, 3: ed position, 4: end line of cycle)
int g_setFunc = 1;  // manual/auto trace in freeze pw(1: no useful, 2: adjust is using, 3: finish to calc auto trace)
int g_tgcSlider[] = {128, 128, 128, 128, 128, 128, 128, 128}; // global variable of tgc control

ViewMain* ViewMain::m_instance = NULL;
bool ViewMain::m_cursorVisible = true;

// ---------------------------------------------------------

void InvisibleCursor(bool invisible, bool resetCursor) {
  if (!invisible) {
    if (resetCursor) {
      SetSystemCursor(SYSCURSOR_X, SYSCUROSR_Y);
    } else {
      ResetIfOutOfRange();
    }
  }

  if (invisible) {
    ChangeSystemCursor(GDK_BLANK_CURSOR);
    ViewMain::GetInstance()->SetCursorVisible(false);
  } else {
    ChangeSystemCursor(GDK_LEFT_PTR);
    ViewMain::GetInstance()->SetCursorVisible(true);
  }
}

void ChangeSystemCursor(GdkCursorType type) {
  GdkDisplay* display = gdk_display_get_default();
  GdkCursor* cursor = gdk_cursor_new_for_display(display, type);
  gdk_window_set_cursor(ViewMain::GetInstance()->GetMainWindow()->window, cursor);
  gdk_cursor_unref(cursor);
}

// ---------------------------------------------------------

ViewMain* ViewMain::GetInstance() {
  if (m_instance == NULL) {
    m_instance = new ViewMain();
  }

  return m_instance;
}

ViewMain::ViewMain() {
  m_window = NULL;
  countN = 0;
  keyTSIN = 0;
}

ViewMain::~ViewMain() {
  if (m_instance != NULL) {
    delete m_instance;
  }

  m_instance = NULL;
}

void ViewMain::Create() {
  // UserSelect
  UserSelect::GetInstance()->create_userconfig_dir();
  UserSelect::GetInstance()->create_userdefined_dir();
  UserSelect::GetInstance()->create_commentdefined_dir();
  UserSelect::GetInstance()->create_usercalcdefined_dir();
  UserSelect::GetInstance()->creat_username_db(USERNAME_DB);
  UserSelect::GetInstance()->save_active_user_id(0);

  // Window
  m_window = Utils::create_app_window("CT", SCREEN_WIDTH, SCREEN_HEIGHT);

  GtkBox* vbox = Utils::create_vbox();
  gtk_container_add(GTK_CONTAINER(m_window), GTK_WIDGET(vbox));

  // TOP
  GtkBox* hbox_top = Utils::create_hbox();

  // Knob Menu Area
  GtkWidget* knobMenu = KnobMenu::GetInstance()->Create();
  gtk_widget_set_size_request(GTK_WIDGET(knobMenu), -1, KNOB_AREA_H);

  // BOTTOM
  GtkBox* hbox_bottom = Utils::create_hbox();
  gtk_widget_set_size_request(GTK_WIDGET(hbox_bottom), -1, HINT_AREA_H);

  gtk_box_pack_start(vbox, GTK_WIDGET(hbox_top), TRUE, TRUE, 0);
  gtk_box_pack_start(vbox, knobMenu, FALSE, FALSE, 0);
  gtk_box_pack_start(vbox, GTK_WIDGET(hbox_bottom), FALSE, FALSE, 0);

  // TOP LEFT
  GtkBox* vbox_top_left = Utils::create_vbox();

  // Menu Area
  GtkWidget* menuArea = MenuArea::GetInstance()->Create();
  gtk_widget_set_size_request(menuArea, MENU_AREA_W, MENU_AREA_H);

  gtk_box_pack_start(hbox_top, GTK_WIDGET(vbox_top_left), TRUE, TRUE, 0);
  gtk_box_pack_start(hbox_top, menuArea, FALSE, FALSE, 0);

  // Hint Area
  GtkWidget* hintArea = HintArea::GetInstance()->Create();
  gtk_widget_set_size_request(hintArea, HINT_AREA_W, -1);

  // Icon
  GtkWidget* viewIcon = ViewIcon::GetInstance()->Create();

  gtk_box_pack_start(hbox_bottom, hintArea, FALSE, FALSE, 0);
  gtk_box_pack_start(hbox_bottom, viewIcon, TRUE, TRUE, 0);

  // Top area
  GtkWidget* topArea = TopArea::GetInstance()->Create(TOP_AREA_W, TOP_AREA_H);
  TopArea::GetInstance()->AddTimeOut();

  // Fixed
  GtkWidget* fixed = gtk_fixed_new();

  gtk_box_pack_start(vbox_top_left, topArea, FALSE, FALSE, 0);
  gtk_box_pack_start(vbox_top_left, fixed, TRUE, TRUE, 0);

  // Image Area
  GtkWidget* imageArea = ImageArea::GetInstance()->Create();

  // Note Area
  GtkWidget* noteArea = NoteArea::GetInstance()->Create();

  gtk_fixed_put(GTK_FIXED(fixed), imageArea, 0, 0);
  gtk_fixed_put(GTK_FIXED(fixed), noteArea, IMAGE_X, IMAGE_Y);

  // 2D knob menu
  KnobD2Create();

  g_keyInterface.Push(this);

  gtk_widget_show_all(GTK_WIDGET(m_window));
  NoteArea::GetInstance()->Hide();

  // update top area
  SysGeneralSetting sysGeneralSetting;
  string hospital_name = sysGeneralSetting.GetHospital();
  TopArea::GetInstance()->UpdateHospitalName(hospital_name);

  // patient info
  g_patientInfo.UpdateTopArea();

  if (g_authorizationOn) {
    CEmpAuthorization::Create(&g_keyInterface, REGISTER_FILE_PATH, 1);
  }
}

void ViewMain::ShowMenu() {
  /*m_daMenu = gtk_drawing_area_new();
  gtk_widget_modify_bg(m_daMenu, GTK_STATE_NORMAL, g_black);
  gtk_drawing_area_size(GTK_DRAWING_AREA(m_daMenu), MENU_AREA_W, MENU_AREA_H - TOP_AREA_H);
  gtk_fixed_put(GTK_FIXED(m_fixedWindow), m_daMenu, 844, TOP_AREA_H);*/
  //gtk_widget_hide(m_daMenu);
}

void ViewMain::HideMenu() {
  //gtk_widget_show(m_daMenu);
}

GtkWidget* ViewMain::GetMainWindow() {
  return GTK_WIDGET(m_window);
}

void ViewMain::KeyEvent(unsigned char keyValue) {
  MonitorControl monitorCtl;

  switch(keyValue) {
  case KEY_BRIGHT_ADD:
    {
      monitorCtl.SetBrightAdd();
      break;
    }
  case KEY_BRIGHT_SUB:
    {
      monitorCtl.SetBrightSub();
      break;
    }
  case KEY_CONTRAST_ADD:
    {
      monitorCtl.SetContrastAdd();
      break;
    }
  case KEY_CONTRAST_SUB:
    {
      monitorCtl.SetContrastSub();
      break;
    }
  case KEY_AUTOADJUST:
    {
      monitorCtl.AutoAdjust();
      break;
    }
  case KEY_AUTOBALANCE:
    {
      monitorCtl.AutoBalance();
      break;
    }
  }

  AbsMultiFunc* ptrMultiFunc = MultiFuncFactory::GetInstance()->GetObject();

  if (MenuArea::GetInstance()->GetMenuType() == MenuArea::REVIEW &&
    ViewSuperuser::GetInstance()->GetDemoStatus()) {
    switch(keyValue) {
    case KEY_SET:
      {
        ptrMultiFunc->Do();
        break;
      }
    case KEY_CURSOR:
      {
        KeyCursor kc;
        kc.Execute();
        break;
      }
    default:
      {
        if (keyValue == 'q') {
          if (ModeStatus::IsUnFreezeMode()) {
            FreezeMode::GetInstance()->PressFreeze();
          }

          ViewSuperuser::GetInstance()->ExitDemo();
          FreezeMode::GetInstance()->PressUnFreeze();
        }

        break;
      }
    }

    m_keyValue = keyValue;

    return;
  }

  ProbeSocket::ProbePara para;
  ProbeMan::GetInstance()->GetCurProbe(para);
  char type = Img2D::GetInstance()->ReviseProbeType(para.type);

  switch (keyValue) {
  case KEY_POWER:
    {
      if (ModeStatus::IsUnFreezeMode()) {
        FreezeMode::GetInstance()->PressFreeze();
      } else if (ModeStatus::IsAutoReplayMode()) {
        FreezeMode::GetInstance()->ChangeAutoReplay();
      } else {
      }

      if (g_authorizationOn) {
        CEmpAuthorization::Destroy();
      }

      KeyPowerOff kpower;
      kpower.Execute();

      break;
    }
  case KEY_FREEZE:
    {
      bool isShowMenuCalc = false;
      bool isShowMenuBiopsy = false;

      if (MenuArea::GetInstance()->GetMenuType() == MenuArea::CALC) {
        isShowMenuCalc = true;
      }

      if (MenuArea::GetInstance()->GetMenuType() == MenuArea::BIOPSY ||
        MenuArea::GetInstance()->GetMenuType() == MenuArea::BIOPSYBRACKET ||
        MenuArea::GetInstance()->GetMenuType() == MenuArea::BIOPSYVERIFY) {
        isShowMenuBiopsy = true;
      }

      if (ModeStatus::IsUnFreezeMode()) {
        FreezeMode::GetInstance()->PressFreeze();
      } else {
        if (ModeStatus::IsPwMode()) {
          if (g_menuPW.GetAutoTraceStatus()) {
            ImgProcPw::GetInstance()->SetAutoCalc(true);
          }

          MeasureMan::GetInstance()->ClearPwTraceUnFreeze();
        }

        FreezeMode::GetInstance()->PressUnFreeze();
      }

      if (isShowMenuCalc) {
        AbsUpdateMix* ptrUpdate = GlobalClassMan::GetInstance()->GetUpdateMix();
        ptrUpdate->EnterCalc();
        MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::NONE);
      }

      if (isShowMenuBiopsy) {
        MenuShowUndo();
        MultiFuncUndo();
      }

      break;
    }
  case KEY_2D:
    {
      KeyLocalZoom zoom;
      zoom.ExitLocalZoom();
      ScanMode::GetInstance()->ExitSpecialMeasure();
      ScanMode::GetInstance()->Enter2D();

      break;
    }
  case KEY_PAGEUP:
    {
      KnobMenu::GetInstance()->PageUp();
      break;
    }
  case KEY_PAGEDOWM:
    {
      KnobMenu::GetInstance()->PageDown();
      break;
    }
  case KEY_F1ADD:
  case KEY_F1SUB:
  case KEY_F2ADD:
  case KEY_F2SUB:
  case KEY_F3ADD:
  case KEY_F3SUB:
  case KEY_F4ADD:
  case KEY_F4SUB:
  case KEY_F5ADD:
  case KEY_F5SUB:
    {
      CKnobEvent::FEvent(keyValue);
      break;
    }
  case KEY_DUAL:
    {
      if (MultiFuncFactory::GetInstance()->GetMultiFuncType() != MultiFuncFactory::PIP_ZOOM) {
        ScanMode::GetInstance()->ExitSpecialMeasure();

        ModeStatus s;
        int mode = s.GetScanMode();

        if (mode == ScanMode::D2) {
          Format2D::EFormat2D format = Format2D::GetInstance()->GetFormat();

          if (format != Format2D::BB) {
            Format2D::GetInstance()->ChangeFormat(Format2D::BB);
          } else {
            Format2D::GetInstance()->SwitchBB();
          }
        } else if ((mode == ScanMode::CFM) || (mode == ScanMode::PDI)) {
          FormatCfm::EFormatCfm format = FormatCfm::GetInstance()->GetFormat();

          if (format != FormatCfm::BB) {
            FormatCfm::GetInstance()->ChangeFormat(FormatCfm::BB);
          } else {
            FormatCfm::GetInstance()->SwitchBB();
          }
        } else if ((mode == ScanMode::CFM_VS_2D) || (mode == ScanMode::PDI_VS_2D)) {
          ScanMode::GetInstance()->EnterCfm();
          FormatCfm::GetInstance()->ChangeFormat(FormatCfm::BB);
        } else {
        }
      }

      break;
    }
  case KEY_QUAD:
    {
      if (MultiFuncFactory::GetInstance()->GetMultiFuncType() != MultiFuncFactory::PIP_ZOOM) {
        ScanMode::GetInstance()->ExitSpecialMeasure();
        ModeStatus s;

        int mode = s.GetScanMode();

        if (mode == ScanMode::D2) {
          Format2D::EFormat2D format = Format2D::GetInstance()->GetFormat();

          if (format != Format2D::B4) {
            Format2D::GetInstance()->ChangeFormat(Format2D::B4);
          } else {
            Format2D::GetInstance()->SwitchB4();
          }
        } else if ((mode == ScanMode::CFM) || (mode == ScanMode::PDI)) {
          FormatCfm::EFormatCfm format = FormatCfm::GetInstance()->GetFormat();

          if (format != FormatCfm::B4) {
            FormatCfm::GetInstance()->ChangeFormat(FormatCfm::B4);
          } else {
            FormatCfm::GetInstance()->SwitchB4();
          }
        } else if ((mode == ScanMode::CFM_VS_2D) || (mode == ScanMode::PDI_VS_2D)) {
          ScanMode::GetInstance()->EnterCfm();
          FormatCfm::GetInstance()->ChangeFormat(FormatCfm::B4);
        }
      }

      break;
    }
  case KEY_P3:
    {
      if (ModeStatus::IsEFOVMode()) {
        HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
        break;
      }

      UserDefineKey ud;
      ud.PressKeyP3();

      break;
    }
  case KEY_FOCUS:
    {
      ModeStatus ms;
      int mode = ms.GetScanMode();

      if ((ModeStatus::IsUnFreezeMode())&&(mode== ScanMode::D2)) {
        if ((ModeStatus::IsMImgMode() && ms.GetFormatM() == FormatM::M_TOTAL) ||
          (ModeStatus::IsPWImgMode() && ms.GetFormatPw() == FormatPw::P_TOTAL)) {
          HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);

          break;
        } else {
          Img2D* ptrImg = Img2D::GetInstance();

          if (ptrImg->IsCompoundSpaceOn() || ptrImg->IsCompoundFreqOn() ||
            ptrImg->GetTpViewStatus() || ptrImg->GetEFVIStatus() ||
            Zoom::GetInstance()->GetLocalZoomStatus()) {
            HintArea::GetInstance()->UpdateHint(_("When zoom, compound imaging, Tp-View or EFVI is on, it only has one focus."), 4);

            break;
          }

          Img2D::GetInstance()->ChangeFocSum(ROTATE);
        }
      }
      break;
    }
  case KEY_UPDATE:
    {
      ptrMultiFunc->Update();
      break;
    }
  case KEY_CW:
    {
      if (type == 'P') {
        ScanMode* ptrS = ScanMode::GetInstance();

        // before enter cw
        ptrS->PrepareEnterCw();

        // enter cw
        ModeStatus s;
        Format2D::EFormat2D format2D = s.GetFormat2D();
        int mode = s.GetScanMode();

        if (mode == ScanMode::CFM && format2D == Format2D::B) {
          ptrS->EnterCwCfmFromCfm();
        } else if ((mode == ScanMode::PDI) && (format2D == Format2D::B)) {
          ptrS->EnterCwPdiFromPdi();
        } else if (mode == ScanMode::CWCFM) {
          ptrS->EnterCfm();
        } else if (mode == ScanMode::CWPDI) {
          ptrS->EnterPdi();
        } else {
          ptrS->EnterCw();
        }
      } else {
        HintArea::GetInstance()->UpdateHint(_("Not realize now!"), 1);
      }

      break;
    }
  case KEY_PDI:
    {
      ScanMode::GetInstance()->ExitSpecialMeasure();
      ScanMode* ptrS = ScanMode::GetInstance();
      ModeStatus s;
      int mode = s.GetScanMode();

      if (Img2D::GetInstance()->GetTpViewStatus() || Img2D::GetInstance()->GetEFVIStatus()) {
        Img2D::GetInstance()->ExitTpView();
        Img2D::GetInstance()->ExitEFVI();
      }

      if (mode == ScanMode::PWPDI) {
        ptrS->UpdatePw();
        g_keyInterface.CtrlLight(FALSE, LIGHT_PDI);
      } else {
        ptrS->EnterPdi();
      }

      break;
    }
  case KEY_PW:
    {
      ScanMode* ptrS = ScanMode::GetInstance();
      ModeStatus s;
      int mode = s.GetScanMode();

      if (mode == ScanMode::PW_INIT || mode == ScanMode::PWCFM_INIT || mode == ScanMode::PWPDI_INIT) {
        ptrMultiFunc->Update();
      } else {
        Format2D::EFormat2D format2D = s.GetFormat2D();

        if (Img2D::GetInstance()->GetTpViewStatus() || Img2D::GetInstance()->GetEFVIStatus()) {
          Img2D::GetInstance()->ExitTpView();
          Img2D::GetInstance()->ExitEFVI();
        }

        if (mode == ScanMode::CFM && format2D == Format2D::B) {
          ptrS->EnterPwCfmFromCfm();
        } else if (mode == ScanMode::PDI && format2D == Format2D::B) {
          ptrS->EnterPwPdiFromPdi();
        } else if (mode == ScanMode::PWCFM || mode == ScanMode::PWCFM_SIMULT) {
          ptrS->EnterCfm();
        } else if (mode == ScanMode::PWPDI || mode == ScanMode::PWPDI_SIMULT) {
          ptrS->EnterPdi();
        } else {
          ptrS->EnterPw();
        }
      }

      HintArea::GetInstance()->UpdateHint(_("[Angle]: <AUTO> to adjust size."), 2);

      break;
    }
  case KEY_CFM:
    {
      ScanMode::GetInstance()->ExitSpecialMeasure();

      ScanMode* ptrS = ScanMode::GetInstance();
      ModeStatus s;
      int mode = s.GetScanMode();

      if (Img2D::GetInstance()->GetTpViewStatus() || Img2D::GetInstance()->GetEFVIStatus()) {
        Img2D::GetInstance()->ExitTpView();
        Img2D::GetInstance()->ExitEFVI();
      }

      if (mode == ScanMode::PWCFM) {
        ptrS->UpdatePw();
        g_keyInterface.CtrlLight(FALSE, LIGHT_CFM);
      } else {
        ptrS->EnterCfm();
      }

      break;
    }
  case KEY_M:
    {
      ModeStatus s;
      int mode = s.GetScanMode();

      if(mode == ScanMode::M_INIT) {
        ptrMultiFunc->Update();
      } else {
        if (mode != ScanMode::M) {
          g_keyInterface.CtrlLight(TRUE, LIGHT_M);
        }

        if (Img2D::GetInstance()->GetTpViewStatus() || Img2D::GetInstance()->GetEFVIStatus()) {
          Img2D::GetInstance()->ExitTpView();
          Img2D::GetInstance()->ExitEFVI();
        }

        ScanMode::GetInstance()->EnterM();
      }

      break;
    }
  case KEY_DEPTHADD:
    {
      if (ModeStatus::IsEFOVMode()) {
        HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);

        break;
      }

      if (ModeStatus::IsUnFreezeMode()) {
        ScanMode::EScanMode mode = ScanMode::GetInstance()->GetFpgaScanMode();

        if (mode == ScanMode::PW || mode == ScanMode::CW) {
          HintArea::GetInstance()->UpdateHint(_("[Depth]: Not valid in current mode."), 1);
        } else {
          KeyDepth kd;
          kd.ExecuteAdd();
        }
      }

      break;
    }
  case KEY_DEPTHSUB:
    {
      if (ModeStatus::IsEFOVMode()) {
        HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);

        break;
      }

      if (ModeStatus::IsUnFreezeMode()) {
        ScanMode::EScanMode mode = ScanMode::GetInstance()->GetFpgaScanMode();

        if ((mode == ScanMode::PW) || (mode == ScanMode::CW)) {
          HintArea::GetInstance()->UpdateHint(_("[Depth]: Not valid in current mode."), 1);
        } else {
          KeyDepth kd;
          kd.ExecuteSub();
        }
      }

      break;
    }
  case KEY_ARCHIVE:
    {
      if (ModeStatus::IsEFOVMode()) {
        HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);

        break;
      }

      if (ModeStatus::IsUnFreezeMode()) {
        FreezeMode::GetInstance()->PressFreeze();
      } else if (ModeStatus::IsAutoReplayMode()) {
        FreezeMode::GetInstance()->ChangeAutoReplay();
      } else {
      }

      ViewArchive::GetInstance()->CreateWindow();

      break;
    }
  case KEY_LOCALZOOM:
    {
      KeyLocalZoom klz;
      klz.Execute();

      break;
    }
  case KEY_ENDEXAM:
    {
      if (ModeStatus::IsEFOVMode()) {
        HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);

        break;
      }

      FreezeMode::GetInstance()->ExitAutoReplay();
      KeyEndExam kee;
      kee.Execute();

      break;
    }
  case KEY_MENU:
    {
      MenuReviewCallBack();

      break;
    }
  case KEY_F1:
  case KEY_F2:
  case KEY_F3:
  case KEY_F4:
  case KEY_F5:
    {
      KnobKeyEvent(keyValue);

      break;
    }
  case KEY_ESC:
    {
      ptrMultiFunc->Undo();

      break;
    }
  case KEY_SET:
    {
      ptrMultiFunc->Do();

      break;
    }
  case KEY_LEFTRIGHT:
    {
      if (ModeStatus::IsEFOVMode() || !ModeStatus::IsUnFreezeMode()) {
        HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);

        break;
      }

      ImgProc2D::GetInstance()->ChangeLR();

      break;
    }
  case KEY_UPDOWN:
    {
      if (ModeStatus::IsEFOVMode() || !ModeStatus::IsUnFreezeMode()) {
        HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);

        break;
      }

      ImgProc2D::GetInstance()->ChangeUD();

      break;
    }
  case KEY_INVERT:
    {
      if (ModeStatus::IsSpectrumImgMode() || ModeStatus::IsSpectrumColorImgMode()) {
        ImgPw::GetInstance()->ChangeInvert();
      }

      break;
    }
  case KEY_MEASURE:
    {
      if (ModeStatus::IsEFOVMode()) {
        ScanMode::EFOVStatus status = ScanMode::GetInstance()->GetEFOVStatus();

        if (status == ScanMode::PREPARE || status == ScanMode::CAPTURE) {
          HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
          break;
        }
      }

      FreezeMode::GetInstance()->ExitAutoReplay();
      KeyMeasure km;
      km.Execute();

      break;
    }
  case KEY_BODYMARK:
    {
      FreezeMode::GetInstance()->ExitAutoReplay();
      KeyBodyMark kbm;
      kbm.Execute();

      break;
    }
  case KEY_UP:
    {
      ptrMultiFunc->KeyUp();

      break;
    }
  case KEY_DOWN:
    {
      ptrMultiFunc->KeyDown();

      break;
    }
  case KEY_LEFT:
    {
      ptrMultiFunc->KeyLeft();

      break;
    }
  case KEY_RIGHT:
    {
      ptrMultiFunc->KeyRight();

      break;
    }
  case KEY_REPORT:
    {
      if (ModeStatus::IsEFOVMode() && !Replay::GetInstance()->GetReadSnapStatus()) {
        HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);

        break;
      }

      if (ModeStatus::IsUnFreezeMode()) {
        FreezeMode::GetInstance()->PressFreeze();
      } else if (ModeStatus::IsAutoReplayMode()) {
        FreezeMode::GetInstance()->ChangeAutoReplay();
      } else {
      }

      ViewReport::GetInstance()->CreateWindow();

      break;
    }
  case KEY_CALC:
    {
      if (ModeStatus::IsEFOVMode()) {
        HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);

        break;
      }

      FreezeMode::GetInstance()->ExitAutoReplay();
      KeyCalc kc;
      kc.Execute();

      break;
    }
  case KEY_SYSTEM:
    {
      if (ModeStatus::IsEFOVMode() && !Replay::GetInstance()->GetReadSnapStatus()) {
        HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);

        break;
      }

      if (ModeStatus::IsAutoReplayMode()) {
        FreezeMode::GetInstance()->ChangeAutoReplay();
      } else if (ModeStatus::IsUnFreezeMode()) {
        FreezeMode::GetInstance()->PressFreeze();
      }

      ViewSystem::GetInstance()->CreateWindow();
      ViewSystem::GetInstance()->ChangeNoteBookPage(0);

      break;
    }
  case KEY_TEXT:
    {
      if (ModeStatus::IsEFOVMode() && !Replay::GetInstance()->GetReadSnapStatus()) {
        ScanMode::EFOVStatus status = ScanMode::GetInstance()->GetEFOVStatus();

        if (status == ScanMode::PREPARE || status == ScanMode::CAPTURE) {
          HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);

          break;
        } else if (status == ScanMode::REVIEW && Replay::GetInstance()->IsEFOVAutoReplay()) {
          Replay::GetInstance()->EFOVAutoReviewEnd();
        }
      }

      if (ModeStatus::IsUnFreezeMode()) {
        FreezeMode::GetInstance()->PressFreeze();
      }

      FreezeMode::GetInstance()->ExitAutoReplay();

      KeyText kt;
      kt.Execute();

      break;
    }
  case KEY_ARROW:
    {
      if (ModeStatus::IsUnFreezeMode()) {
        FreezeMode::GetInstance()->PressFreeze();
      }

      FreezeMode::GetInstance()->ExitAutoReplay();
      KeyArrow ka;
      ka.Execute();

      break;
    }
  case KEY_CLEAR:
    {
      KeyClearScreen kcs;
      kcs.Execute();

      break;
    }
  case KEY_SAVE:
    {
      if (ModeStatus::IsEFOVMode()) {
        ScanMode::EFOVStatus status = ScanMode::GetInstance()->GetEFOVStatus();

        if (status == ScanMode::PREPARE || status == ScanMode::CAPTURE) {
          HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);

          break;
        }
      }

      KeySaveSnap ks;
      ks.Execute();

      break;
    }
  case KEY_P1:
    {
      if (ModeStatus::IsEFOVMode()) {
        HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);

        break;
      }

      UserDefineKey ud;
      ud.PressKeyP1();

      break;
    }
  case KEY_P2:
    {
      if (ModeStatus::IsEFOVMode()) {
        HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);

        break;
      }

      UserDefineKey ud;
      ud.PressKeyP2();

      break;
    }
  case KEY_PROBE:
    {
      if (ModeStatus::IsEFOVMode() && !Replay::GetInstance()->GetReadSnapStatus()) {
        HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);

        break;
      }

      if (ModeStatus::IsUnFreezeMode()) {
        FreezeMode::GetInstance()->PressFreeze();
      }

      FreezeMode::GetInstance()->ExitAutoReplay();
      ViewProbe::GetInstance()->Create();

      break;
    }
  case KEY_PATIENT:
    {
      if (ModeStatus::IsEFOVMode() && !Replay::GetInstance()->GetReadSnapStatus()) {
        HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);

        break;
      }

      if (ModeStatus::IsUnFreezeMode()) {
        FreezeMode::GetInstance()->PressFreeze();
      }

      FreezeMode::GetInstance()->ExitAutoReplay();
      ViewNewPat::GetInstance()->CreateWindow();

      if (ModeStatus::IsPwMode()) {
        if(g_menuPW.GetAutoTraceStatus()) {
          ImgProcPw::GetInstance()->SetAutoCalc(TRUE);
        }

        MeasureMan::GetInstance()->ClearPwTraceUnFreeze();
      }

      FreezeMode::GetInstance()->PressUnFreeze();

      break;
    }
  case KEY_CURSOR:
    {
      KeyCursor kc;
      kc.Execute();

      break;
    }
  case KEY_VOLUMEADD:
    {
      if (ModeStatus::IsEFOVMode()) {
        HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);

        break;
      }

      ImgPw::GetInstance()->ChangeSoundVolume(ADD);

      break;
    }
  case KEY_VOLUMESUB:
    {
      if (ModeStatus::IsEFOVMode()) {
        HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
        break;
      }

      ImgPw::GetInstance()->ChangeSoundVolume(SUB);

      break;
    }
  case KEY_PWRADD:
    {
      if (ModeStatus::IsEFOVMode()) {
        HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);

        break;
      }

      if (ModeStatus::IsUnFreezeMode()) {
        Img2D::GetInstance()->ChangeSoundPower(ADD);
        ImgPw::GetInstance()->ChangeSoundPower(ADD);

        // change tis
        ChangeTis();
      }

      break;
    }
  case KEY_PWRSUB:
    {
      if (ModeStatus::IsEFOVMode()) {
        HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);

        break;
      }

      if (ModeStatus::IsUnFreezeMode()) {
        Img2D::GetInstance()->ChangeSoundPower(SUB);
        ImgPw::GetInstance()->ChangeSoundPower(SUB);

        // change tis
        ChangeTis();
      }

      break;
    }
  case KEY_MBP:
    {
      if (ModeStatus::IsEFOVMode()) {
        HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);

        break;
      }

      if (ModeStatus::IsUnFreezeMode()) {
        ModeStatus s;
        int mode = s.GetScanMode();

        if (mode == ScanMode::CFM || mode == ScanMode::PDI ||
          mode == ScanMode::PWCFM_INIT || mode == ScanMode::PWPDI_INIT ||
          mode == ScanMode::PWCFM || mode == ScanMode::PWPDI ||
          mode == ScanMode::PWCFM_SIMULT || mode == ScanMode::PWPDI_SIMULT) {
          ImgCfm::GetInstance()->ChangeMBP(TRUE);
        } else {
          ImgCfm::GetInstance()->ChangeMBP(FALSE);
        }
      }

      break;
    }
  case KEY_S:
    {
      m_vecSuperAuthenInfo.clear();

      if (m_super_timer > 0) {
        g_source_remove(m_super_timer);
        m_super_timer = 0;
      }

      m_statusSuperAuthen = true;
      m_super_timer = g_timeout_add(5000, signal_callback_superauthen, this);

      break;
    }
  case KEY_SUPER_P:
  case KEY_SUPER_R:
  case KEY_SUPER_1:
  case KEY_SUPER_9:
  case KEY_SUPER_7:
    {
      if (m_statusSuperAuthen) {
        m_vecSuperAuthenInfo.push_back(keyValue);
      }

      break;
    }
  case KEY_D:
    {
      m_vecAuthenInfo.clear();

      if (m_timer > 0) {
        g_source_remove(m_timer);
        m_timer = 0;
      }

      m_vecAuthenInfo.push_back(keyValue);
      m_statusAuthen = TRUE;
      m_timer = g_timeout_add(5000, signal_callback_authen, this);

      break;
    }
  case KEY_I:
  case KEY_C:
    {
      if (m_statusAuthen) {
        m_vecAuthenInfo.push_back(keyValue);
      }

      break;
    }
  case KEY_DICOM_M:
  case KEY_O:
  case KEY_E:
    {
      if (m_statusAuthen) {
        m_vecAuthenInfo.push_back(keyValue);
      }

      if (m_statusSuperAuthen) {
        m_vecSuperAuthenInfo.push_back(keyValue);
      }

      break;
    }
  default:
      break;
  }
}

void ViewMain::KnobKeyEvent(unsigned char keyValue) {
  KnobMenu* knobMenu = KnobMenu::GetInstance();

  switch(keyValue) {
  case KEY_F1:
    {
      knobMenu->Knob1_Press();
      break;
    }
  case KEY_F2:
    {
      knobMenu->Knob2_Press();
      break;
    }
  case KEY_F3:
    {
      knobMenu->Knob3_Press();
      break;
    }
  case KEY_F4:
    {
      knobMenu->Knob4_Press();
      break;
    }
  case KEY_F5:
    {
      knobMenu->Knob5_Press();
      break;
    }
  case KEY_F6:
    {
      knobMenu->Knob6_Press();
      break;
    }
  default:
      break;
  }
}

void ViewMain::MenuReviewCallBack() {
  if (ModeStatus::IsEFOVMode() && !Replay::GetInstance()->GetReadSnapStatus()) {
    if (KeyReview::m_menuRead == TRUE) {
      KeyReview km;
      km.ExitMenuReivew();
    } else {
      HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
    }
  } else {
    if (ModeStatus::IsAutoReplayMode()) {
      FreezeMode::GetInstance()->ChangeAutoReplay();
    }

    FreezeMode::GetInstance()->ExitAutoReplay();
    KeyReview km;

    if(ViewSuperuser::GetInstance()->GetDemoStatus()) {
      km.SetDemoIDAndPath();
    } else {
      km.SetDefaultIDAndPath();
    }

    km.Execute();
  }
}

bool ViewMain::GetCursorVisible() {
  return m_cursorVisible;
}

void ViewMain::SetCursorVisible(bool visible) {
  m_cursorVisible = visible;
}

// ---------------------------------------------------------

void ViewMain::IsAuthenValid() {
  m_statusAuthen = false;

  if(!m_vecAuthenInfo.empty()) {
    unsigned char info[AUTHEN_NUM + 1] = {0};

    for (int i = 0; i < m_vecAuthenInfo.size(); i ++) {
      info[i] = m_vecAuthenInfo[i];
    }

    // clear
    m_vecAuthenInfo.clear();

    // compare
    if (string((char*)info) == "dicome") {
      ViewDicomSupervise::GetInstance()->CreateWindow();
    }
  }
}

void ViewMain::IsSuperAuthenValid() {
  m_statusSuperAuthen = false;

  if(!m_vecSuperAuthenInfo.empty()) {
    unsigned char info[SUPER_AUTHEN_NUM + 1] = {0};

    for (int i = 0; i < m_vecSuperAuthenInfo.size(); i ++) {
      info[i] = m_vecSuperAuthenInfo[i];
    }

    // clear
    m_vecSuperAuthenInfo.clear();

    // compare
    if (string((char*)info) == "emperor1997") {
      ViewSuperuser::GetInstance()->CreateWindow();
    }

    // compare
    if (string((char*)info) == "e1997") {
      ViewSuperuser::GetInstance()->CreateDemoWindow();
    }
  }
}

void ViewMain::KnobEvent(unsigned char keyValue, unsigned char offset) {
  if (offset != 1 && offset != 0) {
    offset = 1;
  }

  EKnobOper knob;

  if (offset == 0) {
    knob = SUB;
  }

  if (offset == 1) {
    knob = ADD;
  }

  if (MenuArea::GetInstance()->GetMenuType() == MenuArea::REVIEW &&
    ViewSuperuser::GetInstance()->GetDemoStatus()) {
    return;
  }

  switch (keyValue) {
  case KNOB_VALUE:
    {
      MultiFuncFactory::GetInstance()->GetObject()->Value(knob);
      break;
    }
  case KNOB_GAIN:
    {
      ModeStatus MStatus;
      int mode = MStatus.GetScanMode();

      if (ModeStatus::IsEFOVMode()) {
        HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
        break;
      }

      if (ModeStatus::IsUnFreezeMode()) {
        if (mode == ScanMode::D2) {
          Img2D::GetInstance()->ChangeGain2D(knob);
        }

        if (mode == ScanMode::M) {
          Img2D::GetInstance()->ChangeGainM(knob);
        }
        if (ModeStatus::IsColorImgMode()) {
          ImgCfm::GetInstance()->ChangeGainCfm(knob);
        }

        if (ModeStatus::IsPWImgMode()) {
          ImgPw::GetInstance()->ChangeGainPw(knob);
        }

        if (ModeStatus::IsCWImgMode()) {
          ImgPw::GetInstance()->ChangeGainCw(knob);
        }
      }

      break;
    }
  default:
    {
      CKnobEvent::KnobEvent(keyValue, offset);
      break;
    }
  }
}
