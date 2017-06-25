#include "imageProc/MenuPW.h"

#include "display/HintArea.h"
#include "display/ImageAreaDraw.h"
#include "imageControl/ImgPw.h"
#include "imageProc/FormatPw.h"
#include "imageProc/ImgProcPw.h"
#include "imageProc/ModeStatus.h"
#include "keyboard/KeyFunc.h"
#include "measure/MeasureMan.h"
#include "measure/UpdateMeasure.h"
#include "sysMan/SysGeneralSetting.h"
#include "sysMan/SysOptions.h"

MenuPW g_menuPW;
MenuPW g_menuCW;

extern bool g_calcPwStatus;

CusSpin::CusSpinItem MenuPW::m_item_fastangle = {N_("Fast Angle"),      "0", OK, ChgFastAngle};
CusSpin::CusSpinItem MenuPW::m_item_timeres   = {N_("Time Resolution"), "1", OK, ChgTimeRes};

// ---------------------------------------------------------

MenuPW::MenuPW() {
  m_table = NULL;
}

MenuPW::~MenuPW() {
}

GtkWidget* MenuPW::Create() {
  GtkTable* m_table = Utils::create_table(17, 8);
  gtk_container_set_border_width(GTK_CONTAINER(m_table), 5);

  // Time Mark
  m_checkbutton_timemark = Utils::create_check_button(_("Time Mark"));
  gtk_table_attach_defaults(m_table, GTK_WIDGET(m_checkbutton_timemark), 0, 8, 0, 1);

  gtk_button_set_focus_on_click(GTK_BUTTON(m_checkbutton_timemark), FALSE);
  g_signal_connect(m_checkbutton_timemark, "clicked", G_CALLBACK(signal_checkbutton_clicked_timemark), this);

  // Auto Calculate
  m_checkbutton_autocalc = Utils::create_check_button(_("Auto Calculate"));
  gtk_table_attach_defaults(m_table, GTK_WIDGET(m_checkbutton_autocalc), 0, 8, 1, 2);

  gtk_button_set_focus_on_click(GTK_BUTTON(m_checkbutton_autocalc), FALSE);
  g_signal_connect(m_checkbutton_autocalc, "clicked", G_CALLBACK(signal_checkbutton_clicked_autocalc), this);

  // Auto Trace
  m_checkbutton_autotrace = Utils::create_check_button(_("Auto Trace"));
  gtk_table_attach_defaults(m_table, GTK_WIDGET(m_checkbutton_autotrace), 0, 8, 2, 3);

  gtk_button_set_focus_on_click(GTK_BUTTON(m_checkbutton_autotrace), FALSE);
  g_signal_connect(m_checkbutton_autotrace, "clicked", G_CALLBACK(signal_checkbutton_clicked_autotrace), this);

  // Fast Angle
  m_spin_fastangle.SetItem(&m_item_fastangle);
  m_fastangle = m_spin_fastangle.Create();
  gtk_table_attach(m_table, m_fastangle, 0, 8, 3, 4, (GtkAttachOptions)(GTK_FILL | GTK_EXPAND), GTK_SHRINK, 0, 0);

  // Time Resolution
  m_spin_timeres.SetItem(&m_item_timeres);
  m_timeres = m_spin_timeres.Create();
  gtk_table_attach(m_table, m_timeres, 0, 8, 4, 5, (GtkAttachOptions)(GTK_FILL | GTK_EXPAND), GTK_SHRINK, 0, 0);

  // Scale Unit
  m_button_scale = Utils::create_button(_("Scale Unit"));
  m_combobox_scale = Utils::create_combobox_text();

  gtk_table_attach(m_table, GTK_WIDGET(m_button_scale), 0, 5, 5, 6, (GtkAttachOptions)(GTK_FILL | GTK_EXPAND), GTK_SHRINK, 0, 0);
  gtk_table_attach(m_table, GTK_WIDGET(m_combobox_scale), 5, 8, 5, 6, (GtkAttachOptions)(GTK_FILL | GTK_EXPAND), GTK_SHRINK, 0, 0);

  gtk_button_set_focus_on_click(m_button_scale, FALSE);
  gtk_widget_modify_bg(GTK_WIDGET(m_button_scale), GTK_STATE_NORMAL, Utils::get_color("black"));

  gtk_combo_box_text_append_text(m_combobox_scale, _("cm/s"));
  gtk_combo_box_text_append_text(m_combobox_scale, _("kHz"));

  SysGeneralSetting sysGeneralSetting;
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_scale), sysGeneralSetting.GetScaleUnit());
  g_signal_connect(m_combobox_scale, "changed", G_CALLBACK(signal_combobox_changed_scale), this);

  // Trace-Type
  m_button_tracetype = Utils::create_button(_("Trace-Type"));
  m_combobox_tracetype = Utils::create_combobox_text();

  gtk_table_attach(m_table, GTK_WIDGET(m_button_tracetype), 0, 5, 6, 7, (GtkAttachOptions)(GTK_FILL | GTK_EXPAND), GTK_SHRINK, 0, 0);
  gtk_table_attach(m_table, GTK_WIDGET(m_combobox_tracetype), 5, 8, 6, 7, (GtkAttachOptions)(GTK_FILL | GTK_EXPAND), GTK_SHRINK, 0, 0);

  gtk_button_set_focus_on_click(m_button_tracetype, FALSE);
  gtk_widget_modify_bg(GTK_WIDGET(m_button_tracetype), GTK_STATE_NORMAL, Utils::get_color("black"));

  gtk_combo_box_text_append_text(m_combobox_tracetype, _("Max"));
  gtk_combo_box_text_append_text(m_combobox_tracetype, _("Mean"));

  g_signal_connect(m_combobox_tracetype, "changed", G_CALLBACK(signal_combobox_changed_tracetype), this);

  // Trace-Direction
  m_button_direction = Utils::create_button(_("Trace-Direction"));
  m_combobox_direction = Utils::create_combobox_text();

  gtk_table_attach(m_table, GTK_WIDGET(m_button_direction), 0, 5, 7, 8, (GtkAttachOptions)(GTK_FILL | GTK_EXPAND), GTK_SHRINK, 0, 0);
  gtk_table_attach(m_table, GTK_WIDGET(m_combobox_direction), 5, 8, 7, 8, (GtkAttachOptions)(GTK_FILL | GTK_EXPAND), GTK_SHRINK, 0, 0);

  gtk_button_set_focus_on_click(m_button_direction, FALSE);
  gtk_widget_modify_bg(GTK_WIDGET(m_button_direction), GTK_STATE_NORMAL, Utils::get_color("black"));

  gtk_combo_box_text_append_text(m_combobox_direction, _("above"));
  gtk_combo_box_text_append_text(m_combobox_direction, _("below"));
  gtk_combo_box_text_append_text(m_combobox_direction, _("both"));

  g_signal_connect(m_combobox_direction, "changed", G_CALLBACK(signal_combobox_changed_direction), this);

  // Trace-Threshold
  m_button_threshold = Utils::create_button(_("Trace-Threshold"));
  m_combobox_threshold = Utils::create_combobox_text();

  gtk_table_attach(m_table, GTK_WIDGET(m_button_threshold), 0, 5, 8, 9, (GtkAttachOptions)(GTK_FILL | GTK_EXPAND), GTK_SHRINK, 0, 0);
  gtk_table_attach(m_table, GTK_WIDGET(m_combobox_threshold), 5, 8, 8, 9, (GtkAttachOptions)(GTK_FILL | GTK_EXPAND), GTK_SHRINK, 0, 0);

  gtk_button_set_focus_on_click(m_button_threshold, FALSE);
  gtk_widget_modify_bg(GTK_WIDGET(m_button_threshold), GTK_STATE_NORMAL, Utils::get_color("black"));

  gtk_combo_box_text_append_text(m_combobox_threshold, _("0"));
  gtk_combo_box_text_append_text(m_combobox_threshold, _("1"));
  gtk_combo_box_text_append_text(m_combobox_threshold, _("2"));
  gtk_combo_box_text_append_text(m_combobox_threshold, _("3"));
  gtk_combo_box_text_append_text(m_combobox_threshold, _("4"));

  g_signal_connect(m_combobox_direction, "changed", G_CALLBACK(signal_combobox_changed_threshold), this);

  // Trace-Smooth
  m_button_smooth = Utils::create_button(_("Trace-Smooth"));
  m_combobox_smooth = Utils::create_combobox_text();

  gtk_table_attach(m_table, GTK_WIDGET(m_button_smooth), 0, 5, 9, 10, (GtkAttachOptions)(GTK_FILL | GTK_EXPAND), GTK_SHRINK, 0, 0);
  gtk_table_attach(m_table, GTK_WIDGET(m_combobox_smooth), 5, 8, 9, 10, (GtkAttachOptions)(GTK_FILL | GTK_EXPAND), GTK_SHRINK, 0, 0);

  gtk_button_set_focus_on_click(m_button_smooth, FALSE);
  gtk_widget_modify_bg(GTK_WIDGET(m_button_smooth), GTK_STATE_NORMAL, Utils::get_color("black"));

  gtk_combo_box_text_append_text(m_combobox_smooth, _("0"));
  gtk_combo_box_text_append_text(m_combobox_smooth, _("1"));
  gtk_combo_box_text_append_text(m_combobox_smooth, _("2"));

  g_signal_connect(m_combobox_smooth, "changed", G_CALLBACK(signal_combobox_changed_smooth), this);

  // Display Format
  m_frame_display = Utils::create_frame(_("Display Format"));
  gtk_table_attach_defaults(m_table, GTK_WIDGET(m_frame_display), 0, 8, 10, 17);

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

void MenuPW::Show() {
  gtk_widget_show_all(GTK_WIDGET(m_table));
  gtk_widget_hide(GTK_WIDGET(m_checkbutton_autotrace));
  gtk_widget_hide(m_fastangle);

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc))) {
    ImgProcPw::GetInstance()->SetAutoCalc(TRUE);
  }

  SysGeneralSetting sysGeneralSetting;

  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_scale), sysGeneralSetting.GetScaleUnit());
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_tracetype), sysGeneralSetting.GetTraceType());
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_direction), sysGeneralSetting.GetDirection());
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_threshold), sysGeneralSetting.GetThreshold());
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_smooth), sysGeneralSetting.GetSmooth());
}

void MenuPW::Hide() {
  gtk_widget_hide_all(GTK_WIDGET(m_table));

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc))) {
    ImgProcPw::GetInstance()->SetAutoCalc(FALSE);
  }
}

void MenuPW::UpdateLabel() {
  gtk_button_set_label(GTK_BUTTON(m_checkbutton_timemark), _("Time Mark"));
  gtk_button_set_label(GTK_BUTTON(m_checkbutton_autocalc), _("Auto Calculate"));
  gtk_button_set_label(GTK_BUTTON(m_checkbutton_autotrace), _("Auto Trace"));
  gtk_button_set_label(m_button_scale, _("Scale Unit"));
  gtk_button_set_label(m_button_tracetype, _("Trace-Type"));
  gtk_button_set_label(m_button_direction, _("Trace-Direction"));
  gtk_button_set_label(m_button_threshold, _("Trace-Threshold"));
  gtk_button_set_label(m_button_smooth, _("Trace-Smooth"));
  gtk_frame_set_label(m_frame_display, _("Display Format"));

  m_spin_fastangle.UpdateLabel();
  m_spin_timeres.UpdateLabel();

  UpdateScale(GTK_COMBO_BOX(m_combobox_scale));
  UpdateTraceType(GTK_COMBO_BOX(m_combobox_tracetype));
  UpdateDirection(GTK_COMBO_BOX(m_combobox_direction));
  UpdateThreshold(GTK_COMBO_BOX(m_combobox_threshold));
  UpdateSmooth(GTK_COMBO_BOX(m_combobox_smooth));
}

void MenuPW::UpdateTimeMark(bool on) {
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_timemark), on);
}

void MenuPW::UpdateAutoCalc(bool on) {
  ImgProcPw::GetInstance()->SetAutoCalc(on);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc), on);
}

void MenuPW::UpdateAutoTrace(bool on) {
}

void MenuPW::UpdateTimeRes(const string str, EKnobReturn status) {
  m_spin_timeres.SetValue(str, status);
}

bool MenuPW::GetAutoTraceStatus() {
  return gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc));
}

void MenuPW::Sensitive(bool on) {
  m_sensitive = on;

  GList* list = gtk_container_get_children(GTK_CONTAINER(m_table));
  if(list != NULL) {
    g_list_foreach(list, (GFunc)(signal_callback_foreach_widget), this);
    g_list_free(list);
  }
}

// ---------------------------------------------------------

void MenuPW::CheckButtonClickedTimeMark(GtkToggleButton* togglebutton) {
  gboolean value = gtk_toggle_button_get_active(togglebutton);
  ImgProcPw::GetInstance()->ChangeTimeMark(value);
}

void MenuPW::CheckButtonClickedAutoCalc(GtkToggleButton* togglebutton) {
  gboolean value = gtk_toggle_button_get_active(togglebutton);

  ImgProcPw::GetInstance()->SetTraceStatus(value);

  if(ModeStatus::IsUnFreezeMode()) {
    ImgProcPw::GetInstance()->SetAutoCalc(value);
  }

  if(!value) {
    ImgProcPw::GetInstance()->SetAutoCalc(value);

    // 测量增加到测量管理中
    if (g_calcPwStatus) {
      UpdateMeasure update;
      update.ClearLast();
      MeasureMan::GetInstance()->ClearLast();

      POINT psP;
      POINT edP;

      psP.x = 0;
      psP.y = CANVAS_AREA_H / 2;

      edP.x = 0;
      edP.y = CANVAS_AREA_H / 2;

      ImageAreaDraw::GetInstance()->SetPsEdValue(psP, edP, 1);
    } else {
      ImageAreaDraw::GetInstance()->ClearAutoCalcPara(); // 只是显示，没有增加到测量管理中
    }

    ImageArea::GetInstance()->ClearTrace();
  }
}

void MenuPW::CheckButtonClickedAutoTrace(GtkToggleButton* togglebutton) {
  gboolean value = gtk_toggle_button_get_active(togglebutton);

  if (value) {
    gtk_widget_show(GTK_WIDGET(m_checkbutton_autotrace));
  } else {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_autotrace), FALSE);
    gtk_widget_hide(GTK_WIDGET(m_checkbutton_autotrace));
    ImageArea::GetInstance()->ClearTrace();
  }

  ImgProcPw::GetInstance()->SetTraceStatus(value);
}

void MenuPW::ComboBoxChangedScale(GtkComboBox* combobox) {
  SysGeneralSetting sysGeneralSetting;
  int scaleunit = gtk_combo_box_get_active(combobox);
  sysGeneralSetting.SetScaleUnit(scaleunit);
  sysGeneralSetting.SyncFile();

  ImageAreaDraw* m_ptrImgDraw = ImageAreaDraw::GetInstance();
  m_ptrImgDraw->SetPwScaleUnit(ImageAreaDraw::SCALEUNIT(gtk_combo_box_get_active(combobox)));
}

void MenuPW::ComboBoxChangedTraceType(GtkComboBox* combobox) {
  SysGeneralSetting sysGeneralSetting;
  int tracetype= gtk_combo_box_get_active(combobox);
  sysGeneralSetting.SetTraceType(tracetype);
  sysGeneralSetting.SyncFile();

  ImgProcPw::GetInstance()->SetTraceType(gtk_combo_box_get_active(combobox));
}

void MenuPW::ComboBoxChangedDirection(GtkComboBox* combobox) {
  SysGeneralSetting sysGeneralSetting;
  int direction = gtk_combo_box_get_active(combobox);
  sysGeneralSetting.SetDirection(direction);
  sysGeneralSetting.SyncFile();

  ImgProcPw::GetInstance()->SetTraceDirection(gtk_combo_box_get_active(combobox));
}

void MenuPW::ComboBoxChangedThreshold(GtkComboBox* combobox) {
  SysGeneralSetting sysGeneralSetting;
  int threshold = gtk_combo_box_get_active(combobox);
  sysGeneralSetting.SetThreshold(threshold);
  sysGeneralSetting.SyncFile();

  ImgProcPw::GetInstance()->SetTraceThreshold(gtk_combo_box_get_active(combobox));
}

void MenuPW::ComboBoxChangedSmooth(GtkComboBox* combobox) {
  SysGeneralSetting sysGeneralSetting;
  int smooth = gtk_combo_box_get_active(combobox);
  sysGeneralSetting.SetSmooth(smooth);
  sysGeneralSetting.SyncFile();

  ImgProcPw::GetInstance()->SetTraceSmooth(gtk_combo_box_get_active(combobox));
}

void MenuPW::ButtonClickedDisplay1(GtkButton* button) {
  if (ModeStatus::IsUnFreezeMode()) {
    if(StatusChangeFormat()) {
      FormatPw::GetInstance()->ChangeFormat(FormatPw::P_TOTAL);
      WriteFormatPw(FormatPw::P_TOTAL);
    } else {
      HintArea::GetInstance()->UpdateHint(_("[Format]: Format change is invalid in current mode."), 2);
    }
  } else {
    HintArea::GetInstance()->UpdateHint(_("[Format]: Format change is invalid in freeze mode."), 2);
  }
}

void MenuPW::ButtonClickedDisplay2(GtkButton* button) {
  if (ModeStatus::IsUnFreezeMode()) {
    if(StatusChangeFormat()) {
      FormatPw::GetInstance()->ChangeFormat(FormatPw::BP11_UD);
      WriteFormatPw(FormatPw::BP11_UD);
    } else {
      HintArea::GetInstance()->UpdateHint(_("[Format]: Format change is invalid in current mode."), 2);
    }
  } else {
    HintArea::GetInstance()->UpdateHint(_("[Format]: Format change is invalid in freeze mode."), 2);
  }
}

void MenuPW::ButtonClickedDisplay3(GtkButton* button) {
  if (ModeStatus::IsUnFreezeMode()) {
    if(StatusChangeFormat()) {
      FormatPw::GetInstance()->ChangeFormat(FormatPw::BP21_UD);
      WriteFormatPw(FormatPw::BP21_UD);
    } else {
      HintArea::GetInstance()->UpdateHint(_("[Format]: Format change is invalid in current mode."), 2);
    }
  } else {
    HintArea::GetInstance()->UpdateHint(_("[Format]: Format change is invalid in freeze mode."), 2);
  }
}

void MenuPW::ButtonClickedDisplay4(GtkButton* button) {
  if (ModeStatus::IsUnFreezeMode()) {
    if(StatusChangeFormat()) {
      FormatPw::GetInstance()->ChangeFormat(FormatPw::BP12_UD);
      WriteFormatPw(FormatPw::BP12_UD);
    } else {
      HintArea::GetInstance()->UpdateHint(_("[Format]: Format change is invalid in current mode."), 2);
    }
  } else {
    HintArea::GetInstance()->UpdateHint(_("[Format]: Format change is invalid in freeze mode."), 2);
  }
}

void MenuPW::ButtonClickedDisplay5(GtkButton* button) {
  if (ModeStatus::IsUnFreezeMode()) {
    if(StatusChangeFormat()) {
      FormatPw::GetInstance()->ChangeFormat(FormatPw::BP11_LR);
      WriteFormatPw(FormatPw::BP11_LR);
    } else {
      HintArea::GetInstance()->UpdateHint(_("[Format]: Format change is disable in current mode."), 2);
    }
  } else {
    HintArea::GetInstance()->UpdateHint(_("[Format]: Format change is invalid in freeze mode."), 2);
  }
}

void MenuPW::CallbackForeachWidget(GtkWidget* widget) {
  if(widget != GTK_WIDGET(m_checkbutton_autocalc) && widget != GTK_WIDGET(m_checkbutton_autotrace)
    && widget != GTK_WIDGET(m_button_scale) && widget != GTK_WIDGET(m_combobox_scale)) {
    gtk_widget_set_sensitive(widget, m_sensitive);
  }
}

void MenuPW::ChgFastAngle(EKnobOper oper) {
  ImgPw::GetInstance()->ChangeCorrectAngleFast(oper);
}

void MenuPW::ChgTimeRes(EKnobOper oper) {
  ImgProcPw::GetInstance()->ChangeTimeSmooth(oper);
}

void MenuPW::UpdateScale(GtkComboBox* combobox) {
  GtkTreeModel* model = gtk_combo_box_get_model(combobox);
  GtkListStore* store = GTK_LIST_STORE(model);

  GtkTreeIter iter;

  gtk_tree_model_get_iter_first(model, &iter);
  gtk_list_store_set(store, &iter, 0, _("cm/s"), -1);
  gtk_tree_model_iter_next(model, &iter);
  gtk_list_store_set(store, &iter, 0, _("kHz"), -1);
}

void MenuPW::UpdateTraceType(GtkComboBox* combobox) {
  GtkTreeModel* model = gtk_combo_box_get_model(combobox);
  GtkListStore* store = GTK_LIST_STORE(model);

  GtkTreeIter iter;

  gtk_tree_model_get_iter_first(model, &iter);
  gtk_list_store_set(store, &iter, 0, _("Max"), -1);
  gtk_tree_model_iter_next(model, &iter);
  gtk_list_store_set(store, &iter, 0, _("Mean"), -1);
}

void MenuPW::UpdateDirection(GtkComboBox* combobox) {
  GtkTreeModel* model = gtk_combo_box_get_model(combobox);
  GtkListStore* store = GTK_LIST_STORE(model);

  GtkTreeIter iter;

  gtk_tree_model_get_iter_first(model, &iter);
  gtk_list_store_set(store, &iter, 0, _("above"), -1);
  gtk_tree_model_iter_next(model, &iter);
  gtk_list_store_set(store, &iter, 0, _("below"), -1);
  gtk_tree_model_iter_next(model, &iter);
  gtk_list_store_set(store, &iter, 0, _("both"), -1);
}

void MenuPW::UpdateThreshold(GtkComboBox* combobox) {
}

void MenuPW::UpdateSmooth(GtkComboBox* combobox) {
}

bool MenuPW::StatusChangeFormat() {
  ModeStatus modeStatus;
  ScanMode::EScanMode mode = modeStatus.GetScanMode();

  if (mode == ScanMode::PW || mode == ScanMode::PWCFM || mode == ScanMode::PWPDI ||
    mode == ScanMode::PW_SIMULT || mode == ScanMode::PWCFM_SIMULT
    || mode == ScanMode::PWPDI_SIMULT || mode == ScanMode::CW
    || mode == ScanMode::CWCFM || mode == ScanMode::CWPDI) {
    return true;
  } else {
    return false;
  }
}

// write Pw format to config file
// format[in]: PW format
void MenuPW::WriteFormatPw(int format) {
  SysOptions sysOpt;
  sysOpt.SetDisplayFormatPW(format);
  sysOpt.SyncFile();
}
