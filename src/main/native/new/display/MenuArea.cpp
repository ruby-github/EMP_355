#include "display/MenuArea.h"

#include "bodymark/MenuBDMK.h"
#include "calcPeople/MenuCalcNew.h"
#include "comment/MenuNote.h"
#include "imageControl/Knob2D.h"
#include "imageControl/KnobCfm.h"
#include "imageControl/KnobM.h"
#include "imageControl/KnobPw.h"
#include "imageProc/Menu2D.h"
#include "imageProc/MenuCFM.h"
#include "imageProc/MenuEFOV.h"
#include "imageProc/MenuM.h"
#include "imageProc/MenuPW.h"
#include "imageProc/MenuReview.h"
#include "imageProc/ModeStatus.h"
#include "keyboard/KeyFunc.h"
#include "measure/MenuMeasure.h"
#include "probe/MenuBiopsy.h"
#include "probe/MenuBiopsyBracket.h"
#include "probe/MenuBiopsyVerify.h"
#include "sysMan/MenuSystem.h"
#include "ViewMain.h"

MenuArea* MenuArea::m_instance = NULL;

void MenuShowUndo() {
  ModeStatus s;
  ScanMode::EScanMode mode = s.GetScanMode();
  MenuArea::EMenuType type = MenuArea::D2;

  if (Replay::GetInstance()->GetReadSnapStatus()) {
    type = MenuArea::REVIEW;
  } else {
    switch(mode) {
    case ScanMode::D2:
    case ScanMode::M_INIT:
    case ScanMode::PW_INIT:
    case ScanMode::CW_INIT:
      type = MenuArea::D2;
      break;
    case ScanMode::M:
    case ScanMode::ANATOMIC_M:
      type = MenuArea::M;
      break;
    case ScanMode::PW:
    case ScanMode::PW_SIMULT:
      type = MenuArea::PW;
      break;
    case ScanMode::CW:
      type = MenuArea::CW;
      break;
    case ScanMode::CFM:
    case ScanMode::PDI:
    case ScanMode::PWCFM_INIT:
    case ScanMode::PWPDI_INIT:
    case ScanMode::CFM_VS_2D:
    case ScanMode::PDI_VS_2D:
    case ScanMode::CWCFM_INIT:
    case ScanMode::CWPDI_INIT:
      type = MenuArea::CFM;
      break;
    case ScanMode::PWCFM:
    case ScanMode::PWCFM_SIMULT:
    case ScanMode::PWPDI:
    case ScanMode::PWPDI_SIMULT:
      type = MenuArea::PWCFM;
      break;
    case ScanMode::CWCFM:
    case ScanMode::CWPDI:
      type = MenuArea::CWCFM;
      break;
    case ScanMode::EFOV:
      type = MenuArea::EFOV;
      break;
    default:
      type = MenuArea::D2;
      break;
    }
  }

  MenuArea::GetInstance()->SwitchMenu(type);
}

// ---------------------------------------------------------

MenuArea* MenuArea::GetInstance() {
  if (m_instance == NULL) {
    m_instance = new MenuArea();
  }

  return m_instance;
}

MenuArea::MenuArea() {
  m_vbox = NULL;
  m_label_sub = NULL;
  m_notebook = NULL;

  m_menuType = D2;
  m_2d_only = true;
  m_menuVisbible = true;
}

MenuArea::~MenuArea() {
  if (m_instance != NULL) {
    delete m_instance;
  }

  m_instance = NULL;
}

GtkWidget* MenuArea::Create() {
  m_vbox = Utils::create_vbox();

  m_label_sub = Utils::create_label(_("Sub Menu"));
  m_notebook = Utils::create_notebook();

  gtk_widget_set_size_request(GTK_WIDGET(m_label_sub), -1, TOP_AREA_H);

  gtk_box_pack_start(m_vbox, GTK_WIDGET(m_label_sub), FALSE, FALSE, 0);
  gtk_box_pack_start(m_vbox, GTK_WIDGET(m_notebook), TRUE, TRUE, 0);

  Utils::set_font(GTK_WIDGET(m_label_sub), "", "bold", 14);
  gtk_misc_set_alignment(GTK_MISC(m_label_sub), 0.5, 0.5);

  gtk_widget_modify_bg(GTK_WIDGET(m_notebook), GTK_STATE_NORMAL, Utils::get_color("black"));
  gtk_notebook_set_scrollable(m_notebook, TRUE);
  gtk_notebook_set_show_tabs(m_notebook, TRUE);

  // 2D
  gtk_notebook_append_page(m_notebook, g_menu2D.Create(), GTK_WIDGET(Utils::create_label(_("2D"))));

  // M
  gtk_notebook_append_page(m_notebook, g_menuM.Create(), GTK_WIDGET(Utils::create_label(_("M"))));

  // PW
  gtk_notebook_append_page(m_notebook, g_menuPW.Create(), GTK_WIDGET(Utils::create_label(_("PW"))));

  // CW
  gtk_notebook_append_page(m_notebook, g_menuCW.Create(), GTK_WIDGET(Utils::create_label(_("CW"))));

  // CFM
  gtk_notebook_append_page(m_notebook, g_menuCFM.Create(), GTK_WIDGET(Utils::create_label(_("CFM"))));

  // Measure2D
  gtk_notebook_append_page(m_notebook, g_menuMeasure.Create(), GTK_WIDGET(Utils::create_label(_("Measure2D"))));

  // BodyMark
  gtk_notebook_append_page(m_notebook, g_menuBDMK.Create(), GTK_WIDGET(Utils::create_label(_("BodyMark"))));

  // Review
  gtk_notebook_append_page(m_notebook, g_menuReview.Create(), GTK_WIDGET(Utils::create_label(_("Review"))));

  // Calculate
  gtk_notebook_append_page(m_notebook, g_menuCalc.Create(), GTK_WIDGET(Utils::create_label(_("Calculate"))));

  // Note
  gtk_notebook_append_page(m_notebook, g_menuNote.Create(), GTK_WIDGET(Utils::create_label(_("Note"))));

  // Biopsy
  gtk_notebook_append_page(m_notebook, g_menuBiopsy.Create(), GTK_WIDGET(Utils::create_label(_("Review"))));

  // BiopsyBracket
  gtk_notebook_append_page(m_notebook, g_menuBiopsyBracket.Create(), GTK_WIDGET(Utils::create_label(_("BiopsyBracket"))));

  // BiopsyVerify
  gtk_notebook_append_page(m_notebook, g_menuBiopsyVerify.Create(), GTK_WIDGET(Utils::create_label(_("BiopsyVerify"))));

  // EFOV
  gtk_notebook_append_page(m_notebook, g_menuEFOV.Create(), GTK_WIDGET(Utils::create_label(_("EFOV"))));

  // System
  gtk_notebook_append_page(m_notebook, MenuSystem::GetInstance()->Create(), GTK_WIDGET(Utils::create_label(_("System"))));

  g_signal_connect(G_OBJECT(m_notebook), "switch-page", G_CALLBACK(signal_notebook_changed), this);
  gtk_notebook_set_current_page(m_notebook, 0);

  return GTK_WIDGET(m_vbox);
}

MenuArea::EMenuType MenuArea::GetMenuType() {
  return m_menuType;
}

int MenuArea::GetMenuIndex() {
  return gtk_notebook_get_current_page(m_notebook);
}

bool MenuArea::GetMenuVisible() {
  return m_menuVisbible;
}

void MenuArea::ShowMenu() {
  m_menuVisbible = true;

  gtk_widget_show(GTK_WIDGET(m_vbox));
  ViewMain::GetInstance()->ShowMenu();
}

void MenuArea::HideMenu() {
  m_menuVisbible = false;

  gtk_widget_hide(GTK_WIDGET(m_vbox));
  ViewMain::GetInstance()->HideMenu();
}

void MenuArea::ShowMainMenu() {
  m_menuType = MAIN;
  gtk_label_set_text(m_label_sub, "");

  HideAllMenu();
}

void MenuArea::Show2DMenu() {
  m_2d_only = true;
  m_menuType = D2;

  HideAllMenu();
  g_menu2D.Show();

  gtk_label_set_text(m_label_sub, _("2D Mode"));

  gtk_notebook_set_show_tabs(m_notebook, TRUE);
  gtk_notebook_set_current_page(m_notebook, 0);
}

void MenuArea::ShowMMenu() {
  m_menuType = M;

  HideAllMenu();
  g_menu2D.Show();
  g_menuM.Show();

  gtk_label_set_text(m_label_sub, _("M Mode"));

  gtk_notebook_set_show_tabs(m_notebook, TRUE);
  gtk_notebook_set_current_page(m_notebook, 1);
}

void MenuArea::ShowPWMenu() {
  m_menuType = PW;

  HideAllMenu();
  g_menu2D.Show();
  g_menuPW.Show();

  if (ModeStatus::IsCWImgMode()) {
    gtk_label_set_text(m_label_sub, _("CW Mode"));
    gtk_notebook_set_tab_label_text(m_notebook, gtk_notebook_get_nth_page(m_notebook, 2), _("CW"));
  } else {
    gtk_label_set_text(m_label_sub, _("PW Mode"));
    gtk_notebook_set_tab_label_text(m_notebook, gtk_notebook_get_nth_page(m_notebook, 2), _("PW"));
  }

  gtk_notebook_set_show_tabs(m_notebook, TRUE);
  gtk_notebook_set_current_page(m_notebook, 2);
}

void MenuArea::ShowCWMenu() {
  m_menuType = CW;

  HideAllMenu();
  g_menu2D.Show();
  g_menuCW.Show();

  gtk_label_set_text(m_label_sub, _("CW Mode"));

  gtk_notebook_set_show_tabs(m_notebook, TRUE);
  gtk_notebook_set_current_page(m_notebook, 3);
}

void MenuArea::ShowCFMMenu() {
  m_menuType = CFM;

  HideAllMenu();
  g_menu2D.Show();
  g_menuCFM.Show();

  gtk_label_set_text(m_label_sub, _("Color Mode"));

  gtk_notebook_set_show_tabs(m_notebook, TRUE);
  gtk_notebook_set_current_page(m_notebook, 4);
}

void MenuArea::ShowPWCFMMenu(bool currentPw) {
  m_menuType = PWCFM;

  HideAllMenu();
  g_menu2D.Show();
  g_menuPW.Show();
  g_menuCFM.Show();

  if (currentPw) {
    if (ModeStatus::IsCWImgMode()) {
      gtk_label_set_text(m_label_sub, _("CW Color Mode"));
      gtk_notebook_set_tab_label_text(m_notebook, gtk_notebook_get_nth_page(m_notebook, 2), _("CW"));
    } else {
      gtk_label_set_text(m_label_sub, _("PW Color Mode"));
      gtk_notebook_set_tab_label_text(m_notebook, gtk_notebook_get_nth_page(m_notebook, 2), _("PW"));
    }

    gtk_notebook_set_show_tabs(m_notebook, TRUE);
    gtk_notebook_set_current_page(m_notebook, 2);
  } else {
    if (ModeStatus::IsCWImgMode()) {
      gtk_label_set_text(m_label_sub, _("Color CW Mode"));
    } else {
      gtk_label_set_text(m_label_sub, _("Color PW Mode"));
    }

    gtk_notebook_set_show_tabs(m_notebook, TRUE);
    gtk_notebook_set_current_page(m_notebook, 4);
  }
}

void MenuArea::ShowCWCFMMenu(bool currentCw) {
  m_menuType = CWCFM;

  HideAllMenu();
  g_menu2D.Show();
  g_menuCW.Show();
  g_menuCFM.Show();

  if (currentCw) {
    gtk_label_set_text(m_label_sub, _("CW Color Mode"));
    gtk_notebook_set_show_tabs(m_notebook, TRUE);

    gtk_notebook_set_current_page(m_notebook, 3);
  } else {
    gtk_label_set_text(m_label_sub, _("CFM Mode"));

    gtk_notebook_set_show_tabs(m_notebook, TRUE);
    gtk_notebook_set_current_page(m_notebook, 4);
  }
}

void MenuArea::ShowMeasureMenu() {
  m_menuType = MEASURE;

  HideAllMenu();
  g_menuMeasure.Show();

  gtk_label_set_text(m_label_sub, _("Measure"));
  gtk_notebook_set_current_page(m_notebook, 5);
}

void MenuArea::ShowBDMKMenu() {
  m_menuType = BDMK;

  HideAllMenu();

  g_menuBDMK.Show();

  gtk_label_set_text(m_label_sub, _("BodyMark"));
  gtk_notebook_set_current_page(m_notebook, 6);
}

void MenuArea::ShowReviewMenu() {
  m_menuType = REVIEW;

  HideAllMenu();
  g_menuReview.Show();

  gtk_label_set_text(m_label_sub, _("Review"));
  gtk_notebook_set_current_page(m_notebook, 7);
}

void MenuArea::ShowCalcMenu() {
  m_menuType = CALC;

  HideAllMenu();
  g_menuCalc.Show();

  gtk_label_set_text(m_label_sub, _("Calculate"));
  gtk_notebook_set_current_page(m_notebook, 8);
}

void MenuArea::ShowNoteMenu() {
  m_menuType = NOTE;

  HideAllMenu();
  g_menuNote.Show();

  gtk_label_set_text(m_label_sub, _("Note"));
  gtk_notebook_set_current_page(m_notebook, 9);
}

void MenuArea::ShowBiopsyMenu() {
  m_menuType = BIOPSY;

  HideAllMenu();
  g_menuBiopsy.Show();

  gtk_label_set_text(m_label_sub, _("Biopsy"));
  gtk_notebook_set_current_page(m_notebook, 10);
}

void MenuArea::ShowBioBracketMenu() {
  m_menuType = BIOPSYBRACKET;

  HideAllMenu();
  g_menuBiopsyBracket.Show();

  gtk_label_set_text(m_label_sub, _("Biopsy Bracket"));
  gtk_notebook_set_current_page(m_notebook, 11);
}

void MenuArea::ShowBioVerifyMenu() {
  m_menuType = BIOPSYVERIFY;

  HideAllMenu();
  g_menuBiopsyVerify.Show();

  gtk_label_set_text(m_label_sub, _("Biopsy Verify"));
  gtk_notebook_set_current_page(m_notebook, 12);
}

void MenuArea::ShowEFOVPrepare() {
  m_menuType = EFOV;

  HideAllMenu();
  g_menuEFOV.ShowPrepare();

  gtk_label_set_text(m_label_sub, _("EFOV Mode\n\nPrepare Status"));
  gtk_notebook_set_current_page(m_notebook, 13);
}

void MenuArea::ShowEFOVCapture() {
  m_menuType = EFOV;

  HideAllMenu();
  g_menuEFOV.ShowCapture();

  gtk_label_set_text(m_label_sub, _("EFOV Mode\n\nCapture Status"));
  gtk_notebook_set_current_page(m_notebook, 13);
}

void MenuArea::ShowEFOVView() {
  m_menuType = EFOV;

  HideAllMenu();
  g_menuEFOV.ShowView();

  gtk_label_set_text(m_label_sub, _("pScape Mode\n\nView Status"));
  gtk_notebook_set_current_page(m_notebook, 13);
}

void MenuArea::ShowEFOVReview() {
  m_menuType = EFOV;

  HideAllMenu();
  g_menuEFOV.ShowReview();

  gtk_label_set_text(m_label_sub, _("pScape Mode\n\nReview Status"));
  gtk_notebook_set_current_page(m_notebook, 13);
}

void MenuArea::ShowSystemMenu() {
  m_menuType = SYSTEM;

  HideAllMenu();
  MenuSystem::GetInstance()->Show();

  gtk_label_set_text(m_label_sub, _("System Setting"));
  gtk_notebook_set_current_page(m_notebook, 14);
}

void MenuArea::SwitchMenu(EMenuType type) {
  switch(type) {
  case MAIN:
    ShowMainMenu();
    break;
  case D2:
    Show2DMenu();
    break;
  case M:
    ShowMMenu();
    break;
  case PW:
    ShowPWMenu();
    break;
  case CW:
    ShowCWMenu();
    break;
  case CFM:
    ShowCFMMenu();
    break;
  case PWCFM:
    ShowPWCFMMenu(TRUE);
    break;
  case CWCFM:
    ShowCWCFMMenu(TRUE);
    break;
  case MEASURE:
    ShowMeasureMenu();
    break;
  case BDMK:
    ShowBDMKMenu();
    break;
  case REVIEW:
    ShowReviewMenu();
    KeyReview::m_menuRead = TRUE;
    break;
  case CALC:
    ShowCalcMenu();
    break;
  case NOTE:
    ShowNoteMenu();
    break;
  case SYSTEM:
    ShowSystemMenu();
    break;
  case EFOV:
    {
      ScanMode::EFOVStatus status = ScanMode::GetInstance()->GetEFOVStatus();

      switch (status) {
      case ScanMode::PREPARE:
        ShowEFOVPrepare();
        break;
      case ScanMode::CAPTURE:
        ShowEFOVCapture();
        break;
      case ScanMode::VIEW:
        ShowEFOVView();
        break;
      case ScanMode::REVIEW:
        ShowEFOVReview();
        break;
      }
    }

    break;
  default:
    break;
  }
}

void MenuArea::UpdateLabel() {
  g_menu2D.UpdateLabel();
  g_menuM.UpdateLabel();
  g_menuPW.UpdateLabel();
  g_menuCW.UpdateLabel();
  g_menuCFM.UpdateLabel();
  g_menuMeasure.UpdateLabel();
  // g_menuBDMK.UpdateLabel();
  g_menuReview.UpdateLabel();
  g_menuCalc.UpdateLabel();
  g_menuNote.UpdateLabel();
  // g_menuBiopsy.UpdateLabel();
  // g_menuBiopsyBracket.UpdateLabel();
  // g_menuBiopsyVerify.UpdateLabel();
  g_menuEFOV.UpdateLabel();
}

// ---------------------------------------------------------

void MenuArea::NotebookChanged(GtkNotebook* notebook, GtkNotebookPage* page, guint page_num) {
  if (m_2d_only) {
    return;
  }

  ModeStatus ms;

  if (ms.IsFreezeMode() || ms.IsAutoReplayMode()) {
    return;
  }

  switch (page_num) {
  case 0:
    KnobD2Create();
    break;
  case 1:
    if (m_menuType == M) {
      KnobMCreate();
    }

    break;
  case 2:
    if (m_menuType == PW || m_menuType == PWCFM || m_menuType == CWCFM) {
      KnobPwCreate();
    }

    break;
  case 3:
    if (m_menuType == CW || m_menuType == CWCFM) {
      KnobPwCreate();
    }

    break;
  case 4:
    if (m_menuType == CFM || m_menuType == PWCFM || m_menuType == CWCFM) {
      KnobCfmCreate();
    }

    break;
  }
}

void MenuArea::HideAllMenu() {
  /*
  g_menu2D.Hide();
  g_menuM.Hide();
  g_menuPW.Hide();
  g_menuCW.Hide();
  g_menuCFM.Hide();
  g_menuMeasure.Hide();
  g_menuBDMK.Hide();
  g_menuReview.Hide();
  g_menuCalc.Hide();
  g_menuNote.Hide();
  g_menuBiopsy.Hide();
  g_menuBiopsyBracket.Hide();
  g_menuBiopsyVerify.Hide();
  g_menuEFOV.Hide();
  MenuSystem::GetInstance()->Hide();

  m_2d_only = false;
  gtk_notebook_set_show_tabs(m_notebook, FALSE);
  */
}
