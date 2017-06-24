#include "imageProc/MenuM.h"

#include "display/HintArea.h"
#include "imageProc/FormatM.h"
#include "imageProc/ImgProc2D.h"
#include "imageProc/ImgProcM.h"
#include "imageProc/ScanMode.h"
#include "sysMan/SysOptions.h"

MenuM g_menuM;

CusSpin::CusSpinItem  MenuM::m_item_line    = {N_("Line aver"),  "1", OK, ChgLine};
CusSpin::CusSpinItem  MenuM::m_item_smooth  = {N_("Smooth"),      "1", OK, ChgSmooth};
CusSpin::CusSpinItem  MenuM::m_item_enhance = {N_("Enhance M"),   "1", OK, ChgEnhance};

// ---------------------------------------------------------

MenuM::MenuM() {
  m_table = NULL;
  m_format = 1;
}

MenuM::~MenuM() {
}

GtkWidget* MenuM::Create() {
  GtkTable* m_table = Utils::create_table(16, 1);
  gtk_container_set_border_width(GTK_CONTAINER(m_table), 5);

  // Polarity
  m_checkbutton_polarity = Utils::create_check_button(_("Polarity"));
  gtk_table_attach_defaults(m_table, GTK_WIDGET(m_checkbutton_polarity), 0, 1, 0, 1);

  g_signal_connect(m_checkbutton_polarity, "clicked", G_CALLBACK(signal_checkbutton_clicked_polarity), this);

  // Time Mark
  m_checkbutton_timemark = Utils::create_check_button(_("Time Mark"));
  gtk_table_attach_defaults(m_table, GTK_WIDGET(m_checkbutton_timemark), 0, 1, 1, 2);

  g_signal_connect(m_checkbutton_timemark, "clicked", G_CALLBACK(signal_checkbutton_clicked_timemark), this);

  // Line aver
  m_spin_line.SetItem(&m_item_line);
  GtkWidget* line = m_spin_line.Create();
  gtk_table_attach(m_table, line, 0, 1, 2, 3, (GtkAttachOptions)(GTK_FILL | GTK_EXPAND), GTK_SHRINK, 0, 0);

  // Smooth: 1-8
  m_spin_smooth.SetItem(&m_item_smooth);
  GtkWidget* smooth = m_spin_smooth.Create();
  gtk_table_attach(m_table, smooth, 0, 1, 3, 4, (GtkAttachOptions)(GTK_FILL | GTK_EXPAND), GTK_SHRINK, 0, 0);

  // Enhance M
  m_spin_enhance.SetItem(&m_item_enhance);
  GtkWidget* enhance = m_spin_enhance.Create();
  gtk_table_attach(m_table, enhance, 0, 1, 4, 5, (GtkAttachOptions)(GTK_FILL | GTK_EXPAND), GTK_SHRINK, 0, 0);

  // Anatomic M
  m_button_anatomic_m = Utils::create_button(_("Anatomic M"));
  gtk_table_attach(m_table, GTK_WIDGET(m_button_anatomic_m), 0, 1, 5, 6, (GtkAttachOptions)(GTK_FILL | GTK_EXPAND), GTK_SHRINK, 0, 0);

  gtk_widget_modify_bg(GTK_WIDGET(m_button_anatomic_m), GTK_STATE_NORMAL, Utils::get_color("black"));
  gtk_button_set_focus_on_click(m_button_anatomic_m, FALSE);
  g_signal_connect(m_button_anatomic_m, "clicked", G_CALLBACK(signal_button_clicked_anatomic_m), this);

  // Display Format
  m_frame_display = Utils::create_frame(_("Display Format"));
  gtk_table_attach_defaults(m_table, GTK_WIDGET(m_frame_display), 0, 1, 6, 13);

  GtkTable* table_display = Utils::create_table(3, 2);
  gtk_container_set_border_width(GTK_CONTAINER(table_display), 5);
  gtk_container_add(GTK_CONTAINER(m_frame_display), GTK_WIDGET(table_display));

  GtkButton* button_display_1 = Utils::create_button();
  GtkButton* button_display_2 = Utils::create_button();
  GtkButton* button_display_3 = Utils::create_button();
  GtkButton* button_display_4 = Utils::create_button();
  GtkButton* button_display_5 = Utils::create_button();

  gtk_table_attach_defaults(table_display, GTK_WIDGET(button_display_1), 0, 1, 0, 1);
  gtk_table_attach_defaults(table_display, GTK_WIDGET(button_display_2), 1, 2, 0, 1);
  gtk_table_attach_defaults(table_display, GTK_WIDGET(button_display_3), 0, 1, 1, 2);
  gtk_table_attach_defaults(table_display, GTK_WIDGET(button_display_4), 1, 2, 1, 2);
  gtk_table_attach_defaults(table_display, GTK_WIDGET(button_display_5), 0, 1, 2, 3);

  Utils::set_button_image(button_display_1, Utils::create_image(string(CFG_RES_PATH) + string("res/btn_format/1.jpg")));
  Utils::set_button_image(button_display_2, Utils::create_image(string(CFG_RES_PATH) + string("res/btn_format/2.jpg")));
  Utils::set_button_image(button_display_3, Utils::create_image(string(CFG_RES_PATH) + string("res/btn_format/3.jpg")));
  Utils::set_button_image(button_display_4, Utils::create_image(string(CFG_RES_PATH) + string("res/btn_format/4.jpg")));
  Utils::set_button_image(button_display_5, Utils::create_image(string(CFG_RES_PATH) + string("res/btn_format/5.jpg")));

  gtk_button_set_focus_on_click(button_display_1, FALSE);
  gtk_button_set_focus_on_click(button_display_2, FALSE);
  gtk_button_set_focus_on_click(button_display_3, FALSE);
  gtk_button_set_focus_on_click(button_display_4, FALSE);
  gtk_button_set_focus_on_click(button_display_5, FALSE);

  gtk_widget_modify_bg(GTK_WIDGET(button_display_1), GTK_STATE_NORMAL, Utils::get_color("black"));
  gtk_widget_modify_bg(GTK_WIDGET(button_display_2), GTK_STATE_NORMAL, Utils::get_color("black"));
  gtk_widget_modify_bg(GTK_WIDGET(button_display_3), GTK_STATE_NORMAL, Utils::get_color("black"));
  gtk_widget_modify_bg(GTK_WIDGET(button_display_4), GTK_STATE_NORMAL, Utils::get_color("black"));
  gtk_widget_modify_bg(GTK_WIDGET(button_display_5), GTK_STATE_NORMAL, Utils::get_color("black"));

  g_signal_connect(button_display_1, "clicked", G_CALLBACK(signal_button_clicked_display_1), this);
  g_signal_connect(button_display_2, "clicked", G_CALLBACK(signal_button_clicked_display_2), this);
  g_signal_connect(button_display_3, "clicked", G_CALLBACK(signal_button_clicked_display_3), this);
  g_signal_connect(button_display_4, "clicked", G_CALLBACK(signal_button_clicked_display_4), this);
  g_signal_connect(button_display_5, "clicked", G_CALLBACK(signal_button_clicked_display_5), this);

  UpdateLabel();

  return GTK_WIDGET(m_table);
}

void MenuM::Show() {
  gtk_widget_show_all(GTK_WIDGET(m_table));
  gtk_widget_hide(GTK_WIDGET(m_checkbutton_polarity));
}

void MenuM::Hide() {
  gtk_widget_hide_all(GTK_WIDGET(m_table));
}

void MenuM::UpdateLabel() {
  gtk_button_set_label(GTK_BUTTON(m_checkbutton_polarity), _("Polarity"));
  gtk_button_set_label(GTK_BUTTON(m_checkbutton_timemark), _("Time Mark"));
  gtk_button_set_label(m_button_anatomic_m, _("Anatomic M"));
  gtk_frame_set_label(m_frame_display, _("Display Format"));

  m_spin_line.UpdateLabel();
  m_spin_smooth.UpdateLabel();
  m_spin_enhance.UpdateLabel();
}

void MenuM::UpdatePolarity(bool on) {
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_polarity),on);
}

void MenuM::UpdateTimeMark(bool on) {
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_timemark), on);
}

void MenuM::UpdateLine(const string str, EKnobReturn status) {
  m_spin_line.SetValue(str, status);
}

void MenuM::UpdateEnhanceM(const string str, EKnobReturn status) {
  m_spin_enhance.SetValue(str, status);
}

void MenuM::UpdateSmooth(const string str, EKnobReturn status) {
  m_spin_smooth.SetValue(str, status);
}

void MenuM::Sensitive(bool on) {
  m_sensitive = on;

  GList* list = gtk_container_get_children(GTK_CONTAINER(m_table));
  if (list != NULL) {
    g_list_foreach(list, (GFunc)(signal_callback_foreach_widget), this);
    g_list_free(list);
  }
}

// ---------------------------------------------------------

void MenuM::CheckButtonClickedPolarity(GtkButton* button) {
  gboolean value = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button));
  ImgProc2D::GetInstance()->ChangePolarity(value);
}

void MenuM::CheckButtonClickedTimemark(GtkButton* button) {
  gboolean value = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button));
  ImgProcM::GetInstance()->ChangeTimeMark(value);
}

void MenuM::ButtonClickedAnatomic(GtkButton* button) {
  ScanMode::GetInstance()->EnterAnatomicM();
}

void MenuM::ButtonClickedDisplay1(GtkButton* button) {
  if (ModeStatus::IsUnFreezeMode()) {
    if (ModeStatus::IsMImgMode()) {
      FormatM::GetInstance()->ChangeFormat(FormatM::M_TOTAL);
      WriteFormatM(FormatM::M_TOTAL);
    } else {
      HintArea::GetInstance()->UpdateHint(_("[Format]: Format change is invalid in current mode."), 2);
    }
  } else {
    HintArea::GetInstance()->UpdateHint(_("[Format]: Format change is invalid in freeze mode."), 2);
  }
}

void MenuM::ButtonClickedDisplay2(GtkButton* button) {
  if (ModeStatus::IsUnFreezeMode()) {
    FormatM::GetInstance()->ChangeFormat(FormatM::BM11_UD);
    WriteFormatM(FormatM::BM11_UD);
  } else {
    HintArea::GetInstance()->UpdateHint(_("[Format]: Format change is invalid in freeze mode."), 2);
  }
}

void MenuM::ButtonClickedDisplay3(GtkButton* button) {
  if (ModeStatus::IsUnFreezeMode()) {
    FormatM::GetInstance()->ChangeFormat(FormatM::BM21_UD);
    WriteFormatM(FormatM::BM21_UD);
  } else {
    HintArea::GetInstance()->UpdateHint(_("[Format]: Format change is invalid in freeze mode."), 2);
  }
}

void MenuM::ButtonClickedDisplay4(GtkButton* button) {
  if (ModeStatus::IsUnFreezeMode()) {
    FormatM::GetInstance()->ChangeFormat(FormatM::BM12_UD);
    WriteFormatM(FormatM::BM12_UD);
  } else {
    HintArea::GetInstance()->UpdateHint(_("[Format]: Format change is invalid in freeze mode."), 2);
  }
}

void MenuM::ButtonClickedDisplay5(GtkButton* button) {
  if (ModeStatus::IsUnFreezeMode()) {
    FormatM::GetInstance()->ChangeFormat(FormatM::BM11_LR);
    WriteFormatM(FormatM::BM11_LR);
  } else {
    HintArea::GetInstance()->UpdateHint(_("[Format]: Format change is invalid in freeze mode."), 2);
  }
}

void MenuM::CallbackForeachWidget(GtkWidget* widget) {
  gtk_widget_set_sensitive(widget, m_sensitive);
}

void MenuM::ChgLine(EKnobOper oper) {
  ImgProc2D::GetInstance()->ChangeLineAver(oper);
}

void MenuM::ChgEnhance(EKnobOper oper) {
  ImgProcM::GetInstance()->ChangeEnhanceM(oper);
}

void MenuM::ChgSmooth(EKnobOper oper) {
  ImgProcM::GetInstance()->ChangeMSmooth(oper);
}

void MenuM::WriteFormatM(int format) {
  SysOptions sp;

  sp.SetDisplayFormatM(format);
  sp.SyncFile();
}
