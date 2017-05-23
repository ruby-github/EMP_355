#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "display/ImageArea.h"
#include "display/KnobMenu.h"
#include "ViewMain.h"
#include "display/gui_global.h"
#include "display/gui_func.h"
#include "display/TopArea.h"
#include "imageControl/Knob2D.h"
#include "comment/NoteArea.h"
#include "probe/ViewProbe.h"
#include "calcPeople/ViewReport.h"
#include "patient/ViewArchive.h"
#include "patient/ViewNewPat.h"
#include "display/ViewIcon.h"
#include "keyboard/KeyDef.h"
#include "keyboard/MultiFuncFactory.h"
#include "keyboard/KeyFunc.h"
#include "imageProc/ImgProc2D.h"
#include "imageControl/KnobPw.h"
#include "imageProc/ModeStatus.h"
#include "imageProc/FreezeMode.h"
#include "imageProc/ScanMode.h"
#include "imageProc/Format2D.h"
#include "imageProc/FormatM.h"
#include "imageProc/FormatPw.h"
#include "imageProc/FormatCfm.h"
#include "Def.h"
#include <pthread.h>
#include "sysMan/SysGeneralSetting.h"
#include "measure/MeasureMan.h"
#include "sysMan/ViewSystem.h"
#include "sysMan/UserDefineKey.h"
#include "display/HintArea.h"
#include "sysMan/ViewSuperuser.h"
#include "imageProc/GlobalClassMan.h"
#include "keyboard/LightDef.h"
#include "probe/ProbeMan.h"
#include "sysMan/ViewDicomSupervise.h"
#include <EmpAuthorization.h>
#include "imageProc/ImgProcPw.h"
#include "imageProc/MenuPW.h"
#include "measure/MeasureD.h"
#include "imageProc/Zoom.h"
#include "periDevice/ManRegister.h"
#include "periDevice/MonitorControl.h"
#include "sysMan/UserSelect.h"           //addec by LL


#include "utils/Utils.h"
#include "utils/FakeXUtils.h"
#include "utils/MainWindowConfig.h"

#include "imageControl/FpgaCtrl2D.h"
extern MenuPW g_menuPW;
// test_Artifact
// #include "imageProc/ImgProcCfm.h"

////////////////////////////[static function]/////////////////////////////
int g_tgcSlider[8] = {128, 128, 128, 128, 128, 128, 128, 128}; // global variable of tgc control

bool ViewMain::m_cursorVisible = true;
extern bool g_authorizationOn;

//manual/auto trace in freeze pw
int g_setFunc = 1; //1: no useful, 2: adjust is using, 3: finish to calc auto trace
//current operation (1: start line of cycle, 2: ps positoin, 3: ed position, 4: end line of cycle)
int g_curOper = 1;
/**
 * 1: 【Set】is 确认in pw freeze,
 * 2: 【Set】is 校正
 * 3:  not in manual or trace in freeze pw
 */

// only for test
gboolean test(gpointer data) {
  if (!ModeStatus::IsFreezeMode()) {
    FreezeMode::GetInstance()->PressFreeze();
  }

  ViewSystem::GetInstance()->CreateWindow();

  return FALSE;
}

gboolean ClickArchive(gpointer data) {
  if (ModeStatus::IsUnFreezeMode()) {
    FreezeMode::GetInstance()->PressFreeze();
  }

  FreezeMode::GetInstance()->ExitAutoReplay();
  ViewArchive::GetInstance()->CreateWindow();

  return FALSE;
}

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
  GtkWidget* widget = ViewMain::GetInstance()->GetMainWindow();
  GdkDisplay* display = gdk_display_get_default();
  GdkCursor* cursor = gdk_cursor_new_for_display(display, type);
  gdk_window_set_cursor(widget->window, cursor);
  gdk_cursor_unref(cursor);
}

void TestMode(unsigned char keyValue) {
  switch(keyValue) {
  case 'q':
    break;
  case 'a':
    break;
  case 'w':
    break;
  }
}

void ViewMain::KeyEvent(unsigned char keyValue) {
  KnobMenu* ptrKnob = KnobMenu::GetInstance();
  AbsMultiFunc* ptrMultiFunc = MultiFuncFactory::GetInstance()->GetObject();

  ProbeSocket::ProbePara para;
  ProbeMan::GetInstance()->GetCurProbe(para);
  char type = Img2D::GetInstance()->ReviseProbeType(para.type);

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
      ptrKnob->PageUp();

      break;
    }
  case KEY_PAGEDOWM:
    {
      ptrKnob->PageDown();

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

      m_statusSuperAuthen = TRUE;
      m_super_timer = g_timeout_add(5000, HandleSuperAuthen, NULL);

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
      m_timer = g_timeout_add(5000, HandleAuthen, NULL);

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

gboolean ViewMain::IsAuthenValid() {
  m_statusAuthen = false;

  if (m_timer > 0) {
    g_source_remove(m_timer);
    m_timer = 0;
  }

  if(!m_vecAuthenInfo.empty()) {
    unsigned char info[AUTHEN_NUM+1] = {0};

    for (int i = 0; i < m_vecAuthenInfo.size(); i ++) {
      info[i] = m_vecAuthenInfo[i];
    }

    // clear
    m_vecAuthenInfo.clear();

    // compare
    if (strcmp((const char*)info, "dicome") == 0) {
      ViewDicomSupervise::GetInstance()->CreateWindow();
    }
  }

  return FALSE;
}

gboolean ViewMain::IsSuperAuthenValid(void) {
  m_statusSuperAuthen = false;

  if (m_super_timer > 0) {
    g_source_remove(m_super_timer);
    m_super_timer = 0;
  }

  if(!m_vecSuperAuthenInfo.empty()) {
    unsigned char info[SUPER_AUTHEN_NUM+1] = {0};

    for (int i = 0; i < m_vecSuperAuthenInfo.size(); i ++) {
      info[i] = m_vecSuperAuthenInfo[i];
    }

    // clear
    m_vecSuperAuthenInfo.clear();

    // compare
    if (strcmp((const char*)info, "emperor1997") == 0) {
      ViewSuperuser::GetInstance()->CreateWindow();
    }

    // compare
    if (strcmp((const char*)info, "e1997") == 0) {
      ViewSuperuser::GetInstance()->CreateDemoWindow();
    }
  }

  return FALSE;
}

void ViewMain::KnobEvent(unsigned char keyValue, unsigned char offset) {
  AbsMultiFunc* ptrMultiFunc = MultiFuncFactory::GetInstance()->GetObject();
  ModeStatus s;
  ScanMode::EScanMode mode = s.GetScanMode();

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

  //close knob event in demo. lhm
  if (MenuArea::GetInstance()->GetMenuType() == MenuArea::REVIEW &&
    ViewSuperuser::GetInstance()->GetDemoStatus()) {
    return;
  }

  switch (keyValue) {
  case KNOB_VALUE:
    {
      ptrMultiFunc->Value(knob);
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

int g_tgcTimer = 0;

gboolean TgcCallBack(gpointer data) {
  ModeStatus s;
  int mode = s.GetScanMode();

  Img2D::GetInstance()->ChangeTgc2D(g_tgcSlider);

  if (mode == ScanMode::M || mode == ScanMode::M_INIT) {
    Img2D::GetInstance()->ChangeTgcM(g_tgcSlider);
  }

  g_tgcTimer = 0;

  return false;
}

void ViewMain::SliderEvent(unsigned char keyValue, unsigned char offset) {
  if (MenuArea::GetInstance()->GetMenuType() == MenuArea::REVIEW &&
    ViewSuperuser::GetInstance()->GetDemoStatus()) {

    return;
  }

  if (ModeStatus::IsEFOVMode()) {
    HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);

    if (offset >= SLIDER_OFF && offset <= (SLIDER_OFF + 8)) {
      g_tgcSlider[offset - SLIDER_OFF] = keyValue;
    }

    return ;
  }

  if (offset >= SLIDER_OFF && offset <= (SLIDER_OFF + 7)) {
    g_tgcSlider[offset - SLIDER_OFF] = keyValue;

    // remove timer if is start-up
    if (g_tgcTimer > 0) {
      if (!g_source_remove(g_tgcTimer)) {
        PRINTF("remove tgc timer error[%d]\n", offset-SLIDER_OFF);

        return;
      }

      g_tgcTimer = 0;
    }

    // start-up timer
    int interval = 10;
    g_tgcTimer = g_timeout_add(interval, TgcCallBack, NULL);
  }
}

void ViewMain::MouseEvent(char offsetX, char offsetY) {
  AbsMultiFunc* ptrMulti = MultiFuncFactory::GetInstance()->GetObject();
  ptrMulti->Mouse(offsetX, offsetY);
}

void ViewMain::KnobKeyEvent(unsigned char keyValue) {
  KnobMenu* ptrKnob = KnobMenu::GetInstance();

  switch(keyValue) {
  case KEY_F1:
    {
      ptrKnob->Knob1_Press();
      break;
    }
  case KEY_F2:
    {
      ptrKnob->Knob2_Press();
      break;
    }
  case KEY_F3:
    {
      ptrKnob->Knob3_Press();
      break;
    }
  case KEY_F4:
    {
      ptrKnob->Knob4_Press();
      break;
    }
  case KEY_F5:
    {
      ptrKnob->Knob5_Press();
      break;
    }
  default:
      break;
  }

}

gboolean UpdateTopArea(gpointer data) {
  TopArea *tmp;
  tmp = (TopArea *)data;
  tmp->DrawDateTime();

  return TRUE;
}

////////////////////////////[ViewMain]/////////////////////////////
ViewMain* ViewMain::m_ptrInstance = NULL;

ViewMain::ViewMain() {
  m_mainWindow = 0;
  countN = 0;
  m_ptrKnob = KnobMenu::GetInstance();
  m_ptrImgArea = ImageArea::GetInstance();
  m_ptrTopArea = TopArea::GetInstance();
  m_ptrHintArea = HintArea::GetInstance();
  m_ptrMenuArea = MenuArea::GetInstance();
  m_ptrNoteArea = NoteArea::GetInstance();

  keyTSIN = 0;
}

ViewMain::~ViewMain() {
  if (m_ptrInstance != NULL) {
    delete m_ptrInstance;
  }

  m_ptrInstance = NULL;
}

ViewMain* ViewMain::GetInstance() {
  if (m_ptrInstance == NULL) {
    m_ptrInstance = new ViewMain();
  }

  return m_ptrInstance;
}

GtkWidget* ViewMain::GetMainWindow() {
  return m_mainWindow;
}

void ViewMain::Show() {
  gtk_widget_show_all(m_mainWindow);
}

void ViewMain::Hide() {
  gtk_widget_hide_all(m_mainWindow);
}

void ViewMain::ShowMenu() {
  gtk_widget_hide(m_daMenu);
}

void ViewMain::HideMenu() {
  gtk_widget_show(m_daMenu);
}

void ViewMain::Create() {
  UserSelect::GetInstance()->create_userconfig_dir();//addec by LL
  UserSelect::GetInstance()->create_userdefined_dir();
  UserSelect::GetInstance()->create_commentdefined_dir();
  UserSelect::GetInstance()->create_usercalcdefined_dir();
  UserSelect::GetInstance()->creat_username_db(USERNAME_DB);
  UserSelect::GetInstance()->save_active_user_id(0);

  m_mainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  //gtk_window_set_decorated(GTK_WINDOW(m_mainWindow), FALSE);
  gtk_window_set_position(GTK_WINDOW(m_mainWindow), GTK_WIN_POS_CENTER);
  //gtk_window_set_resizable(GTK_WINDOW(m_mainWindow), FALSE);
  gtk_widget_set_size_request(m_mainWindow, SCREEN_WIDTH, SCREEN_HEIGHT);
  gtk_container_set_border_width(GTK_CONTAINER(m_mainWindow), 0);
  gtk_widget_modify_bg(m_mainWindow, GTK_STATE_NORMAL, g_deep);

  //  g_signal_connect(m_mainWindow,"key-press-event",G_CALLBACK(HandleKeyPressEvent),this);

  m_fixedWindow = gtk_fixed_new();
  gtk_widget_set_usize(m_fixedWindow, SCREEN_WIDTH, SCREEN_HEIGHT);
  gtk_widget_set_uposition(m_fixedWindow, 0, 0);
  gtk_container_add(GTK_CONTAINER(m_mainWindow), m_fixedWindow);

  // Top area
  GtkWidget *da_topArea;
  da_topArea = m_ptrTopArea->Create();
  gtk_fixed_put(GTK_FIXED(m_fixedWindow), da_topArea, TOP_AREA_X, TOP_AREA_Y);
  m_ptrTopArea->AddTimeOut();

  // image area
  GtkWidget *da_image;
  da_image = m_ptrImgArea->Create();
  gtk_fixed_put(GTK_FIXED(m_fixedWindow), da_image, IMG_AREA_X, IMG_AREA_Y);
  m_ptrImgArea->AddTimeOutFps();

  // note area
  GtkWidget *canvas_note;
  canvas_note = m_ptrNoteArea->Create();
  gtk_fixed_put(GTK_FIXED(m_fixedWindow), canvas_note, IMG_AREA_X + IMAGE_X, IMG_AREA_Y + IMAGE_Y);

  // Knob Area
  GtkWidget *tableKnob;
  tableKnob = m_ptrKnob->Create();
  gtk_fixed_put(GTK_FIXED(m_fixedWindow), tableKnob, KNOB_AREA_X, KNOB_AREA_Y);

  // hint
  GtkWidget *da_hintArea;
  da_hintArea = m_ptrHintArea->Create();
  gtk_fixed_put(GTK_FIXED(m_fixedWindow), da_hintArea, HINT_AREA_X, HINT_AREA_Y);

  // icon view
  ViewIcon::GetInstance()->Create();
  gtk_fixed_put(GTK_FIXED(m_fixedWindow), ViewIcon::GetInstance()->GetIconArea(), 844, 733); // 740

  // Menu Area
  GtkWidget *tableMenu;
  tableMenu = m_ptrMenuArea->Create();
  gtk_fixed_put(GTK_FIXED(m_fixedWindow), tableMenu, 844, 0);

  m_daMenu = gtk_drawing_area_new();
  gtk_widget_modify_bg(m_daMenu, GTK_STATE_NORMAL, g_black);
  gtk_drawing_area_size(GTK_DRAWING_AREA(m_daMenu), MENU_AREA_W, MENU_AREA_H - TOP_AREA_H);
  gtk_fixed_put(GTK_FIXED(m_fixedWindow), m_daMenu, 844, TOP_AREA_H);

  // 2D knob menu
  KnobD2Create();

  g_keyInterface.Push(this);

  Show();
  gtk_widget_hide(m_daMenu);

  m_ptrNoteArea->Hide();

  // update top area
  SysGeneralSetting *sysGeneralSetting = new SysGeneralSetting;
  string hospital_name = sysGeneralSetting->GetHospital();
  delete sysGeneralSetting;

  m_ptrTopArea->UpdateHospitalName(hospital_name.c_str());

  // patient info
  g_patientInfo.UpdateTopArea();

  if (g_authorizationOn) {
    CEmpAuthorization::Create(&g_keyInterface, REGISTER_FILE_PATH, 1);
  }

  Utils::test(m_mainWindow);
}

void ViewMain::MySleep(int msecond) {
  int sum = Img2D::GetInstance()->GetFocSum();
  usleep(msecond * sum);
}
