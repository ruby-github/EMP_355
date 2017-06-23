#include "imageProc/Menu2D.h"

#include "display/HintArea.h"
#include "imageControl/Img2D.h"
#include "imageProc/ImgProc2D.h"
#include "keyboard/KeyFunc.h"
#include "keyboard/MultiFuncFactory.h"
#include "measure/CDrawIMT.h"

Menu2D g_menu2D;

CusSpin::CusSpinItem Menu2D::m_item_rotate  = {N_("Rotate"),      "0", OK, ChgRotate};
CusSpin::CusSpinItem Menu2D::m_item_frame   = {N_("Frame aver."), "1", OK, ChgFrame};
CusSpin::CusSpinItem Menu2D::m_item_line    = {N_("Line aver."),  "1", OK, ChgLine};
CusSpin::CusSpinItem Menu2D::m_item_smooth  = {N_("Smooth"),      "1", OK, ChgSmooth};
CusSpin::CusSpinItem Menu2D::m_item_gamma   = {N_("Gamma"),       "1", OK, ChgGamma};
CusSpin::CusSpinItem Menu2D::m_item_ipurity = {N_("iPurity"),     "0", OK, ChgIPurity};

// ---------------------------------------------------------

Menu2D::Menu2D() {
  m_table = NULL;
}

Menu2D::~Menu2D() {
}

GtkWidget* Menu2D::Create() {
  GtkTable* m_table = Utils::create_table(16, 1);
  gtk_container_set_border_width(GTK_CONTAINER(m_table), 5);

  // Polarity
  m_checkbutton_polarity = Utils::create_check_button(_("Polarity"));
  gtk_table_attach_defaults(m_table, GTK_WIDGET(m_checkbutton_polarity), 0, 1, 0, 1);

  g_signal_connect(m_checkbutton_polarity, "clicked", G_CALLBACK(signal_checkbutton_clicked_polarity), this);

  // Noise Reject: ON/OFF
  m_checkbutton_restric = Utils::create_check_button(_("Noise Reject"));
  gtk_table_attach_defaults(m_table, GTK_WIDGET(m_checkbutton_restric), 0, 1, 1, 2);

  g_signal_connect(m_checkbutton_restric, "clicked", G_CALLBACK(signal_checkbutton_clicked_restric), this);

  // Rotate: 90/270/OFF
  m_spin_rotate.SetItem(&m_item_rotate);
  GtkWidget* rotate = m_spin_rotate.Create();
  gtk_table_attach(m_table, rotate, 0, 1, 3, 4, GTK_FILL, GTK_SHRINK, 0, 0);

  // Frame aver: 1-8
  m_spin_frame.SetItem(&m_item_frame);
  GtkWidget* frame = m_spin_frame.Create();
  gtk_table_attach(m_table, frame, 0, 1, 4, 5, GTK_FILL, GTK_SHRINK, 0, 0);

  // Line aver: 1-4
  m_spin_line.SetItem(&m_item_line);
  GtkWidget* line = m_spin_line.Create();
  gtk_table_attach(m_table, line, 0, 1, 5, 6, GTK_FILL, GTK_SHRINK, 0, 0);

  // Smooth: 1-8
  m_spin_smooth.SetItem(&m_item_smooth);
  GtkWidget* smooth = m_spin_smooth.Create();
  gtk_table_attach(m_table, smooth, 0, 1, 6, 7, GTK_FILL, GTK_SHRINK, 0, 0);

  // Gamma: 1-8
  m_spin_gamma.SetItem(&m_item_gamma);
  GtkWidget* gamma = m_spin_gamma.Create();
  gtk_table_attach(m_table, gamma, 0, 1, 7, 8, GTK_FILL, GTK_SHRINK, 0, 0);

  // iPurity: 1-5
  m_spin_ipurity.SetItem(&m_item_ipurity);
  GtkWidget* ipurity = m_spin_ipurity.Create();
  gtk_table_attach(m_table, ipurity, 0, 1, 8, 9, GTK_FILL, GTK_SHRINK, 0, 0);

  // Gray Trans
  m_button_gray_trans = Utils::create_button(_("Gray Trans"));
  gtk_table_attach(m_table, GTK_WIDGET(m_button_gray_trans), 0, 1, 9, 10, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_widget_modify_bg(GTK_WIDGET(m_button_gray_trans), GTK_STATE_NORMAL, Utils::get_color("black"));
  gtk_button_set_focus_on_click(m_button_gray_trans, FALSE);
  g_signal_connect(m_button_gray_trans, "clicked", G_CALLBACK(signal_button_clicked_trans), this);

  // Gray Rejection
  m_button_gray_rejection = Utils::create_button(_("Gray Rejection"));
  gtk_table_attach(m_table, GTK_WIDGET(m_button_gray_rejection), 0, 1, 10, 11, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_widget_modify_bg(GTK_WIDGET(m_button_gray_rejection), GTK_STATE_NORMAL, Utils::get_color("black"));
  gtk_button_set_focus_on_click(m_button_gray_rejection, FALSE);
  g_signal_connect(m_button_gray_rejection, "clicked", G_CALLBACK(signal_button_clicked_reject), this);

  // Global Zoom
  m_button_global_zoom = Utils::create_button(_("Global Zoom"));
  gtk_table_attach(m_table, GTK_WIDGET(m_button_global_zoom), 0, 1, 11, 12, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_widget_modify_bg(GTK_WIDGET(m_button_global_zoom), GTK_STATE_NORMAL, Utils::get_color("black"));
  gtk_button_set_focus_on_click(m_button_global_zoom, FALSE);
  g_signal_connect(m_button_global_zoom, "clicked", G_CALLBACK(signal_button_clicked_globalzoom), this);

  // Picture In Picture
  m_button_pip = Utils::create_button(_("Picture In Picture"));
  gtk_table_attach(m_table, GTK_WIDGET(m_button_pip), 0, 1, 12, 13, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_widget_modify_bg(GTK_WIDGET(m_button_pip), GTK_STATE_NORMAL, Utils::get_color("black"));
  gtk_button_set_focus_on_click(m_button_pip, FALSE);
  g_signal_connect(m_button_pip, "clicked", G_CALLBACK(signal_button_clicked_pip), this);

  // 4B
  m_button_4b = Utils::create_button(_("4B"));
  gtk_table_attach(m_table, GTK_WIDGET(m_button_4b), 0, 1, 13, 14, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_widget_modify_bg(GTK_WIDGET(m_button_4b), GTK_STATE_NORMAL, Utils::get_color("black"));
  gtk_button_set_focus_on_click(m_button_4b, FALSE);
  g_signal_connect(m_button_4b, "clicked", G_CALLBACK(signal_button_clicked_4b), this);

  // pScape Mode
  m_button_efov = Utils::create_button(_("pScape Mode"));
  gtk_table_attach(m_table, GTK_WIDGET(m_button_efov), 0, 1, 14, 15, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_widget_modify_bg(GTK_WIDGET(m_button_efov), GTK_STATE_NORMAL, Utils::get_color("black"));
  gtk_button_set_focus_on_click(m_button_efov, FALSE);
  g_signal_connect(m_button_efov, "clicked", G_CALLBACK(signal_button_clicked_efov), this);

  return GTK_WIDGET(m_table);
}

void Menu2D::Show() {
  gtk_widget_show_all(GTK_WIDGET(m_table));

  if (ModeStatus::IsSpectrumColorMode()) {
    gtk_widget_hide(GTK_WIDGET(m_button_4b));
  }

  gtk_widget_hide(GTK_WIDGET(m_checkbutton_polarity));
}

void Menu2D::Hide() {
  gtk_widget_hide_all(GTK_WIDGET(m_table));
}

void Menu2D::UpdateLabel() {
  gtk_button_set_label(GTK_BUTTON(m_checkbutton_polarity), _("Polarity"));
  gtk_button_set_label(GTK_BUTTON(m_checkbutton_restric), _("Noise Reject"));
  gtk_button_set_label(m_button_gray_trans, _("Gray Trans"));
  gtk_button_set_label(m_button_gray_rejection, _("Gray Rejection"));
  gtk_button_set_label(m_button_global_zoom, _("Global Zoom"));
  gtk_button_set_label(m_button_pip, _("Picture In Picture"));
  gtk_button_set_label(m_button_4b, _("4B"));
  gtk_button_set_label(m_button_efov, _("pScape Mode"));

  m_spin_rotate.UpdateLabel();
  m_spin_frame.UpdateLabel();
  m_spin_line.UpdateLabel();
  m_spin_smooth.UpdateLabel();
  m_spin_gamma.UpdateLabel();
  m_spin_ipurity.UpdateLabel();
}

void Menu2D::UpdatePolarity(bool on) {
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_polarity), on);
}

void Menu2D::UpdateNoiseReject(bool on) {
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_restric), on);
}

void Menu2D::UpdateFrame(const string str, EKnobReturn status) {
  m_spin_frame.SetValue(str, status);
}

void Menu2D::UpdateRotate(const string str, EKnobReturn status) {
  m_spin_rotate.SetValue(str, status);
}

void Menu2D::UpdateLine(const string str, EKnobReturn status) {
  m_spin_line.SetValue(str, status);
}

void Menu2D::UpdateSmooth(const string str, EKnobReturn status) {
  m_spin_smooth.SetValue(str, status);
}

void Menu2D::UpdateGamma(const string str, EKnobReturn status) {
  m_spin_gamma.SetValue(str, status);
}

void Menu2D::UpdateIPurity(const string str, EKnobReturn status) {
  m_spin_ipurity.SetValue(str, status);
}

void Menu2D::Sensitive(bool on) {
  m_sensitive = on;

  GList* list =  gtk_container_get_children(GTK_CONTAINER(m_table));

  if (list != NULL) {
    g_list_foreach(list, (GFunc)(signal_callback_foreach_widget), this);
    g_list_free(list);
  }
}

void Menu2D::ButtonClickedPIP(GtkButton* button) {
  ModeStatus ms;

  if (ms.IsD2Mode() && (ms.GetFormat2D() == Format2D::B) && ms.IsUnFreezeMode()) {
    MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::PIP_ZOOM);
  } else {
    HintArea::GetInstance()->UpdateHint(_("[PIP]: Only valid in 2D B mode and UnFreeze status."), 1);
  }
}

void Menu2D::ButtonClicked4B(GtkButton* button) {
  if (MultiFuncFactory::GetInstance()->GetMultiFuncType() != MultiFuncFactory::PIP_ZOOM) {
    ModeStatus s;
    int mode = s.GetScanMode();

    if (Img2D::GetInstance()->GetTpViewStatus() || Img2D::GetInstance()->GetEFVIStatus()) {
      Img2D::GetInstance()->ExitTpView();
      Img2D::GetInstance()->ExitEFVI();
    }

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
    } else if (mode == ScanMode::CFM_VS_2D) {
      ScanMode::GetInstance()->EnterCfm();
      FormatCfm::GetInstance()->ChangeFormat(FormatCfm::B4);
    } else if (mode == ScanMode::PDI_VS_2D) {
      ScanMode::GetInstance()->EnterPdi();
      FormatCfm::GetInstance()->ChangeFormat(FormatCfm::B4);
    }
  }
}

// ---------------------------------------------------------

void Menu2D::CheckButtonClickedPolarity(GtkButton* button) {
  gboolean value = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button));
  ImgProc2D::GetInstance()->ChangePolarity(value);
}

void Menu2D::CheckButtonClickedNoiseReject(GtkButton* button) {
  gboolean value = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button));
  ImgProc2D::GetInstance()->ChangeNoiseReject(value);
}

void Menu2D::ButtonClickedTrans(GtkButton* button) {
  MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::GRAY_TRANS);
}

void Menu2D::ButtonClickedReject(GtkButton* button) {
  MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::GRAY_REJECT);
}

void Menu2D::ButtonClickedGlobalZoom(GtkButton* button) {
  int rotate = ImgProc2D::GetInstance()->GetRotate();
  ModeStatus ms;

  if (ms.IsD2Mode() && (ms.GetFormat2D() == Format2D::B) && ms.IsUnFreezeMode()) {
    if (rotate == 0) {
      MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::GLOBAL_ZOOM);
    } else {
      HintArea::GetInstance()->UpdateHint(_("[Global Zoom]: Invalid when image is rotated."), 1);
    }
  } else {
    HintArea::GetInstance()->UpdateHint(_("[Global Zoom]: Only valid in 2D B mode and UnFreeze status."), 1);
  }
}

void Menu2D::ButtonClickedEnterEFOV(GtkButton* button) {
  ModeStatus ms;

  if (DscMan::GetInstance()->GetDscPara()->dcaProbeType == 'L' && ms.IsUnFreezeMode() && ModeStatus::IsD2Mode()) {
    // exit local zoom if in it
    KeyLocalZoom klz;
    klz.ExitLocalZoom();

    // exit Tp-View if in it
    Img2D::GetInstance()->ExitTpView();

    // enter EFOV mode
    ScanMode::GetInstance()->EnterEFOV();
  } else {
    HintArea::GetInstance()->UpdateHint(_("[eView]: Only valid in 2D mode, unfreeze status, linear probe."), 2);
  }
}

void Menu2D::ChgRotate(EKnobOper oper) {
  ModeStatus ms;

  if ((ms.IsD2Mode() && (ms.GetFormat2D() == Format2D::B))) {
    ImgProc2D::GetInstance()->ChangeRotate(oper);
  } else {
    HintArea::GetInstance()->UpdateHint(_("[Rotate]: Only valid in 2D B mode."), 1);
  }
}

void Menu2D::ChgFrame(EKnobOper oper) {
  ImgProc2D::GetInstance()->ChangeFrameAver(oper);
}

void Menu2D::ChgLine(EKnobOper oper) {
  ImgProc2D::GetInstance()->ChangeLineAver(oper);
}

void Menu2D::ChgSmooth(EKnobOper oper) {
  ImgProc2D::GetInstance()->ChangeSmooth(oper);
}

void Menu2D::ChgGamma(EKnobOper oper) {
  ImgProc2D::GetInstance()->ChangeGamma(oper);
}

void Menu2D::ChgIPurity(EKnobOper oper) {
  ImgProc2D::GetInstance()->ChangeImgEhn(oper);
}

void Menu2D::CallbackForeachWidget(GtkWidget* widget) {
  if (widget != GTK_WIDGET(m_button_4b)) {
    gtk_widget_set_sensitive(widget, m_sensitive);
  }
}
